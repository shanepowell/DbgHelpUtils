#include "heap_vs_subsegment.h"

#include "common_symbol_names.h"
#include "heap_vs_entry.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_vs_subsegment::symbol_name = common_symbol_names::heap_vs_subsegment_structure_symbol_name;

    heap_vs_subsegment::heap_vs_subsegment(segment_heap const& heap, uint64_t const heap_vs_subsegment_address)
    : heap_{heap}
    , heap_vs_subsegment_address_{heap_vs_subsegment_address}
    , heap_vs_subsegment_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , heap_vs_subsegment_length_{stream_utils::get_type_length(heap_vs_subsegment_symbol_type_, symbol_name)}
    , heap_vs_chunk_header_length_{stream_utils::get_type_length(stream_utils::get_type(walker(), heap_vs_entry::symbol_name), heap_vs_entry::symbol_name)}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_vs_subsegment::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_vs_subsegment::peb() const
    {
        return heap().peb();
    }

    size_units::base_10::bytes heap_vs_subsegment::size() const
    {
        return size_units::base_10::bytes{static_cast<uint64_t>(size_raw()) << 4};
    }

    uint16_t heap_vs_subsegment::signature() const
    {
        return stream_utils::get_bit_field_value<uint16_t>(*this, common_symbol_names::heap_vs_subsegment_signature_field_symbol_name);
    }

    bool heap_vs_subsegment::is_signature_valid() const
    {
        return (signature() ^ size_raw() ^ 0x2BED) == 0;
    }

    bool heap_vs_subsegment::full_commit() const
    {
        return stream_utils::get_bit_field_value<uint16_t>(*this, common_symbol_names::heap_vs_subsegment_full_commit_field_symbol_name) == 0x01;
    }

    std::experimental::generator<heap_vs_entry> heap_vs_subsegment::entries() const
    {
        uint16_t previous_size = 0;
        auto chunk_header_address = heap_vs_subsegment_address() + heap_vs_subsegment_length_ + get_subsegment_offset();
        const auto last_entry_address = heap_vs_subsegment_address() + size().count();

        while(chunk_header_address < last_entry_address)
        {
            auto buffer = std::make_unique<uint8_t[]>(heap_vs_chunk_header_length_);
            auto const* entry_data = walker().get_process_memory(chunk_header_address, heap_vs_chunk_header_length_);
            if(entry_data == nullptr)
            {
                co_yield heap_vs_entry{heap(), chunk_header_address, last_entry_address - chunk_header_address};
                break;
            }

            memcpy(buffer.get(), entry_data, heap_vs_chunk_header_length_);

            heap().decode_vs_check_header(chunk_header_address, buffer.get());

            heap_vs_entry entry{heap(), chunk_header_address, std::move(buffer), previous_size};

            co_yield entry;

            chunk_header_address += entry.size().count();
            previous_size = entry.raw_size();
        }
    }

    uint16_t heap_vs_subsegment::size_raw() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, common_symbol_names::heap_vs_subsegment_size_field_symbol_name);
    }

    uint64_t heap_vs_subsegment::get_subsegment_offset() const
    {
        if(peb().is_x64_target())
        {
            return 0x08;
        }

        return 0x0;
    }
}
