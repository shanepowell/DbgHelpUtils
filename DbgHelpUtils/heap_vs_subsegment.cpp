#include "heap_vs_subsegment.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "heap_vs_entry.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_vs_subsegment::symbol_name = common_symbol_names::heap_vs_subsegment_structure_symbol_name;

    heap_vs_subsegment::heap_vs_subsegment(segment_heap const& heap, uint64_t const heap_vs_subsegment_address)
    : cache_data_{heap.cache().get_cache<cache_data>()}
    , heap_{heap}
    , heap_vs_subsegment_address_{heap_vs_subsegment_address}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& heap_vs_subsegment::walker() const
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
        return stream_utils::get_bit_field_value<uint16_t>(*this, cache_data_.heap_vs_subsegment_signature_field_data, common_symbol_names::heap_vs_subsegment_signature_field_symbol_name);
    }

    bool heap_vs_subsegment::is_signature_valid() const
    {
        return (signature() ^ last_entry_offset() ^ 0x2BED) == 0;
    }

    bool heap_vs_subsegment::full_commit() const
    {
        return stream_utils::get_bit_field_value<uint16_t>(*this, cache_data_.heap_vs_subsegment_full_commit_field_data, common_symbol_names::heap_vs_subsegment_full_commit_field_symbol_name) == 0x01;
    }

    std::experimental::generator<heap_vs_entry> heap_vs_subsegment::entries() const
    {
        uint16_t previous_size = 0;
        auto chunk_header_address = heap_vs_subsegment_address() + cache_data_.heap_vs_subsegment_length + get_subsegment_offset();
        const auto last_entry_address = heap_vs_subsegment_address() + size().count();

        while(chunk_header_address < last_entry_address)
        {
            auto buffer = std::make_unique<uint8_t[]>(cache_data_.heap_vs_chunk_header_length);
            if(auto stream = walker().get_process_memory_stream(chunk_header_address, cache_data_.heap_vs_chunk_header_length);
                stream.eof() || stream.read(buffer.get(), cache_data_.heap_vs_chunk_header_length) != cache_data_.heap_vs_chunk_header_length)
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

    void heap_vs_subsegment::setup_globals(segment_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_vs_subsegment_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.heap_vs_subsegment_length = stream_utils::get_type_length(data.heap_vs_subsegment_symbol_type, symbol_name);
            data.heap_vs_chunk_header_length = stream_utils::get_type_length(stream_utils::get_type(heap.walker(), heap_vs_entry::symbol_name), heap_vs_entry::symbol_name);

            data.heap_vs_subsegment_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_vs_subsegment_symbol_type, symbol_name, common_symbol_names::heap_vs_subsegment_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);

            data.heap_vs_subsegment_signature_field_data = data.heap_vs_subsegment_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_subsegment_signature_field_symbol_name);
            data.heap_vs_subsegment_full_commit_field_data = data.heap_vs_subsegment_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_subsegment_full_commit_field_symbol_name);
        }
    }

    uint16_t heap_vs_subsegment::last_entry_offset() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, cache_data_.heap_vs_subsegment_size_field_data, common_symbol_names::heap_vs_subsegment_size_field_symbol_name);
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
