#include "heap_segment_context.h"

#include "common_symbol_names.h"
#include "heap_page_segment.h"
#include "list_entry_walker.h"
#include "page_range_descriptor.h"
#include "rtl_rb_tree_walker.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_segment_context::symbol_name = common_symbol_names::heap_seg_context_structure_symbol_name;

    heap_segment_context::heap_segment_context(segment_heap const& heap, uint64_t const heap_segment_context_address)
    : heap_{heap}
    , heap_segment_context_address_{heap_segment_context_address}
    , heap_seg_context_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_segment_context::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_segment_context::peb() const
    {
        return heap().peb();
    }

    size_units::base_16::bytes heap_segment_context::max_allocation_size() const
    {
        return size_units::base_16::bytes{stream_utils::get_field_value<uint32_t>(*this, common_symbol_names::segment_heap_seg_context_max_allocation_size_field_symbol_name)};
    }

    uint64_t heap_segment_context::segment_count() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::segment_heap_seg_context_segment_count_field_symbol_name);
    }

    uint64_t heap_segment_context::segment_mask() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::segment_heap_seg_context_segment_mask_field_symbol_name);
    }

    uint8_t heap_segment_context::unit_shift() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::segment_heap_seg_context_unit_shift_field_symbol_name);
    }

    uint8_t heap_segment_context::pages_per_unit_shift() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::segment_heap_seg_context_pages_per_unit_shift_field_symbol_name);
    }

    uint64_t heap_segment_context::heap_key() const
    {
        return heap().heap_key();
    }

    ust_address_stack_trace const& heap_segment_context::stack_trace() const
    {
        return heap().stack_trace();
    }

    std::experimental::generator<heap_page_segment> heap_segment_context::pages() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{walker(), stream_utils::get_field_address(*this, common_symbol_names::segment_heap_seg_context_segment_list_head_field_symbol_name), heap_page_segment::symbol_name, common_symbol_names::heap_page_segment_list_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_page_segment{*this, entry_address, heap_segment_context_address()};
        }
    }

    std::experimental::generator<page_range_descriptor> heap_segment_context::free_page_ranges() const
    {
        auto const heap_page_range_descriptor_length = stream_utils::get_type_length(stream_utils::get_type(walker(), page_range_descriptor::symbol_name), page_range_descriptor::symbol_name);
        auto const segment_address_mask = segment_mask();

        for(ntdll_utilities::rtl_rb_tree_walker const rb_tree_walker{walker()
            , heap_segment_context_address() + stream_utils::get_field_offset(heap_seg_context_symbol_type_, symbol_name, common_symbol_names::segment_heap_seg_context_free_page_ranges_field_symbol_name)
            , page_range_descriptor::symbol_name
            , common_symbol_names::heap_page_range_descriptor_tree_node_field_symbol_name};
            auto const entry_address : rb_tree_walker.entries())
        {
            auto const page_segment_address = entry_address & segment_address_mask;
            auto const index = (entry_address - page_segment_address) / heap_page_range_descriptor_length;
            co_yield page_range_descriptor{*this, entry_address, index, page_segment_address};
        }
    }
}
