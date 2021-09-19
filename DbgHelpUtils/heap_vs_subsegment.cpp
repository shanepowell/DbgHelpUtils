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

    size_units::base_16::bytes heap_vs_subsegment::size() const
    {
        auto size = static_cast<uint64_t>(last_entry_offset()) << 4;

        // round up to nearest page size...
        auto const page_size = peb().page_size();
        size = (size + (page_size - 1)) / page_size * page_size;

        return size_units::base_16::bytes{size};
    }

    uint16_t heap_vs_subsegment::signature() const
    {
        return stream_utils::get_bit_field_value<uint16_t>(*this, common_symbol_names::heap_vs_subsegment_signature_field_symbol_name);
    }

    bool heap_vs_subsegment::is_signature_valid() const
    {
        return (signature() ^ last_entry_offset() ^ 0x2BED) == 0;
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
            if(auto stream = walker().get_process_memory_stream(chunk_header_address, heap_vs_chunk_header_length_);
                stream.eof() || stream.read(buffer.get(), heap_vs_chunk_header_length_) != heap_vs_chunk_header_length_)
            {
                co_yield heap_vs_entry{heap(), chunk_header_address, last_entry_address - chunk_header_address};
                break;
            }

            heap().decode_vs_check_header(chunk_header_address, buffer.get());

            heap_vs_entry entry{heap(), chunk_header_address, std::move(buffer), previous_size};

            co_yield entry;

            chunk_header_address += entry.size().count();
            previous_size = entry.raw_size();
        }
    }

    uint16_t heap_vs_subsegment::last_entry_offset() const
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
