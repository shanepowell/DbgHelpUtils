#include "heap_vs_context.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "heap_vs_entry.h"
#include "heap_vs_subsegment.h"
#include "list_entry_walker.h"
#include "rtl_rb_tree_walker.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_vs_context::symbol_name = common_symbol_names::heap_vs_context_structure_symbol_name;
    std::wstring const& heap_vs_context::free_chunk_symbol_name = common_symbol_names::heap_vs_chunk_free_header_structure_symbol_name;

    heap_vs_context::heap_vs_context(segment_heap const& heap, uint64_t const heap_vs_context_address)
    : cache_data_{heap.cache().get_cache<cache_data>()}
    , heap_{heap}
    , heap_vs_context_address_{heap_vs_context_address}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& heap_vs_context::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_vs_context::peb() const
    {
        return heap().peb();
    }

    uint64_t heap_vs_context::total_committed_units() const
    {
        return get_machine_size_field_value(*this, cache_data_.heap_vs_context_total_committed_units_field_data, common_symbol_names::heap_vs_context_total_committed_units_field_symbol_name);
    }

    uint64_t heap_vs_context::free_committed_units() const
    {
        return get_machine_size_field_value(*this, cache_data_.heap_vs_context_free_committed_units_field_data, common_symbol_names::heap_vs_context_free_committed_units_field_symbol_name);
    }

    std::experimental::generator<heap_vs_subsegment> heap_vs_context::subsegments() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{heap().cache(), walker(), 
            stream_utils::get_field_address(*this, cache_data_.heap_vs_context_subsegment_list_field_data, common_symbol_names::heap_vs_context_subsegment_list_field_symbol_name),
            heap_vs_subsegment::symbol_name,
            common_symbol_names::heap_vs_subsegment_list_entry_field_symbol_name,
            [](uint64_t const address, uint64_t const parent_address){ return address ^ parent_address; }}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_vs_subsegment{heap_, entry_address};
        }
    }

    std::experimental::generator<heap_vs_entry> heap_vs_context::free_entries() const
    {
        for(ntdll_utilities::rtl_rb_tree_walker const rb_tree_walker{heap().cache(), walker()
            , heap_vs_context_address() + stream_utils::get_field_offset(cache_data_.heap_vs_context_free_chunk_tree_field_data, symbol_name, common_symbol_names::heap_vs_context_free_chunk_tree_field_symbol_name)
            , free_chunk_symbol_name
            , common_symbol_names::heap_vs_chunk_free_header_node_field_symbol_name};
            auto const entry_address : rb_tree_walker.entries())
        {
            auto buffer = std::make_unique<uint8_t[]>(cache_data_.heap_vs_chunk_header_length);
            if(auto stream = walker().get_process_memory_stream(entry_address, cache_data_.heap_vs_chunk_header_length);
                stream.eof() || stream.read(buffer.get(), cache_data_.heap_vs_chunk_header_length) != cache_data_.heap_vs_chunk_header_length)
            {
                continue;
            }

            heap().decode_vs_check_header(entry_address, buffer.get());

            co_yield heap_vs_entry{heap(), entry_address, std::move(buffer)};
        }
    }

    void heap_vs_context::setup_globals(segment_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_vs_context_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.heap_vs_chunk_free_symbol_type = stream_utils::get_type(heap.walker(), free_chunk_symbol_name);
            data.heap_vs_chunk_header_length = stream_utils::get_type_length(stream_utils::get_type(heap.walker(), heap_vs_entry::symbol_name), heap_vs_entry::symbol_name);

            data.heap_vs_context_total_committed_units_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_vs_context_symbol_type, symbol_name, common_symbol_names::heap_vs_context_total_committed_units_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_vs_context_free_committed_units_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_vs_context_symbol_type, symbol_name, common_symbol_names::heap_vs_context_free_committed_units_field_symbol_name, dbg_help::sym_tag_enum::BaseType);

            data.heap_vs_context_subsegment_list_field_data = data.heap_vs_context_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_context_subsegment_list_field_symbol_name);
            data.heap_vs_context_free_chunk_tree_field_data = data.heap_vs_context_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_context_free_chunk_tree_field_symbol_name);
        }
    }
}
