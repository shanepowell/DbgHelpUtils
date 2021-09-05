#include "heap_vs_context.h"

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
    : heap_{heap}
    , heap_vs_context_address_{heap_vs_context_address}
    , heap_vs_context_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , heap_vs_chunk_free_symbol_type_{stream_utils::get_type(walker(), free_chunk_symbol_name)}
    , heap_vs_chunk_header_length_{stream_utils::get_type_length(stream_utils::get_type(walker(), heap_vs_entry::symbol_name), heap_vs_entry::symbol_name)}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_vs_context::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_vs_context::peb() const
    {
        return heap().peb();
    }

    uint64_t heap_vs_context::total_committed_units() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::heap_vs_context_total_committed_units_field_symbol_name);
    }

    uint64_t heap_vs_context::free_committed_units() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::heap_vs_context_free_committed_units_field_symbol_name);
    }

    std::experimental::generator<heap_vs_subsegment> heap_vs_context::subsegments() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{walker(), 
            stream_utils::get_field_address(*this, common_symbol_names::heap_vs_context_subsegment_list_field_symbol_name),
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
        for(ntdll_utilities::rtl_rb_tree_walker const rb_tree_walker{walker()
            , heap_vs_context_address() + stream_utils::get_field_offset(heap_vs_context_symbol_type_, symbol_name, common_symbol_names::heap_vs_context_free_chunk_tree_field_symbol_name)
            , free_chunk_symbol_name
            , common_symbol_names::heap_vs_chunk_free_header_node_field_symbol_name};
            auto const entry_address : rb_tree_walker.entries())
        {
            auto buffer = std::make_unique<uint8_t[]>(heap_vs_chunk_header_length_);
            auto const* entry_data = walker().get_process_memory(entry_address, heap_vs_chunk_header_length_);
            if(entry_data == nullptr)
            {
                continue;
            }

            memcpy(buffer.get(), entry_data, heap_vs_chunk_header_length_);

            heap().decode_vs_check_header(entry_address, buffer.get());

            co_yield heap_vs_entry{heap(), entry_address, std::move(buffer)};
        }
    }
}
