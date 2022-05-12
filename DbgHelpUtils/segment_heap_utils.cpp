#include "segment_heap_utils.h"

#include "process_environment_block.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils::heap
{
    segment_heap_entry_extra_data segment_heap_utils::read_extra_data(process::process_environment_block const& peb, ust_address_stack_trace const& stack_trace, uint64_t const block_address, uint64_t const block_size)
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
        return {unused_bytes, read_ust_address(peb, stack_trace, block_address, block_size, unused_bytes)};
    }

    uint64_t segment_heap_utils::read_ust_address(process::process_environment_block const& peb, ust_address_stack_trace const& stack_trace, uint64_t const block_address, uint64_t const block_size, uint64_t const unused_bytes)
    {
        if(!ust_address_stack_trace::has_ust_setup(peb))
        {
            return 0;
        }

        // find the UST data marker...
        auto const end_address = block_address + block_size;
        auto const ust_field_address = end_address - unused_bytes;

        // this can cut across memory boundaries in the memory list, so support breaking the search up into chunks
        auto stream = peb.walker().get_process_memory_stream(ust_field_address, unused_bytes);
        while(!stream.eof())
        {
            if(!stream.find_pattern<uint8_t>([](uint8_t const data, size_t const found_index, size_t&) { return data == (found_index < 2 ? 0x01 : 0x0); }
                , [header_size = peb.is_x64_target() ? static_cast<size_t>(0xE) : static_cast<size_t>(0x6)](size_t const found_index) { return found_index == header_size; }))
            {
                return 0;
            }

            if(const auto ust_address = stream_utils::read_machine_size_field_value(peb, stream.current_address()); ust_address.has_value())
            {
                // is valid pointer?
                if(stack_trace.is_valid_ust_address(peb, ust_address.value()))
                {
                    return ust_address.value();
                }
            }
        }

        return 0;
    }
}
