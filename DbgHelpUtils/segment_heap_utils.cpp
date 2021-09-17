#include "segment_heap_utils.h"

#include "common_symbol_names.h"
#include "process_environment_block.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils::heap
{
    uint64_t segment_heap_utils::read_front_padding_size(process::process_environment_block const& peb, uint64_t const block_address)
    {
        return stream_utils::read_field_value<uint32_t>(peb.walker(), block_address).value_or(0);
    }

    segment_heap_entry_extra_data segment_heap_utils::read_extra_data(process::process_environment_block const& peb, uint64_t const block_address, uint64_t const block_size)
    {
        auto const unused_bytes_address = block_address + block_size - 0x02;
        auto const unused_bytes_value = stream_utils::read_field_value<uint16_t>(peb.walker(), unused_bytes_address);
        if(!unused_bytes_value.has_value())
        {
            return {0, 0};
        }

        if((unused_bytes_value.value() & 0x8000) == 0x8000)
        {
            // only 1 byte extra data
            return {1, 0};
        }

        // is UST data available
        if((unused_bytes_value.value() & 0x4000) != 0x4000)
        {
            return {unused_bytes_value.value(), 0};
        }

        auto const unused_bytes = static_cast<uint64_t>(unused_bytes_value.value() & 0x3FFF);

        return {unused_bytes, read_ust_address(peb, block_address, block_size, unused_bytes)};
    }

    uint64_t segment_heap_utils::read_ust_address(process::process_environment_block const& peb, uint64_t const block_address, uint64_t const block_size, uint64_t const unused_bytes)
    {
        // find the UST data marker...
        auto const end_address = block_address + block_size;
        auto ust_field_address = end_address - unused_bytes;

        // this can cut across memory boundaries in the memory list, so support breaking the search up into chunks
        auto stream = peb.walker().get_process_memory_stream(ust_field_address, end_address - ust_field_address);
        if(stream.eof())
        {
            return 0;
        }

        auto reset_stream = stream;
        auto reset_ust_field_address = ust_field_address;

        while(!stream.eof())
        {
            // search for 0x01 0x01 0x00 0x00 0x00 0x00 sequence that seems to be the start of the UST data area
            size_t found_marker = 0;
            const size_t header_size = peb.is_x64_target() ? 0xE : 0x6;
            while(ust_field_address < end_address && found_marker < header_size)
            {
                uint8_t data;
                if(stream.read(&data, 1) != 1)
                {
                    return 0;
                }

                if(data == (found_marker < 2 ? 0x01 : 0x0))
                {
                    if(found_marker == 0)
                    {
                        reset_stream = stream;
                        reset_ust_field_address = ust_field_address;
                    }
                    ++found_marker;
                }
                else
                {
                    if(found_marker > 0)
                    {
                        stream = reset_stream;
                        ust_field_address = reset_ust_field_address;
                    }
                    found_marker = 0;
                }
                ++ust_field_address;
            }

            if(found_marker != header_size)
            {
                return 0;
            }

            if(const auto ust_address = stream_utils::read_machine_size_field_value(peb, ust_field_address); ust_address.has_value())
            {
                // is valid pointer?
                if(ust_address_stack_trace::is_valid_ust_address(peb, ust_address.value()))
                {
                    return ust_address.value();
                }
            }
        }

        return 0;
    }
}
