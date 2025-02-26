#pragma once
#include <cstdint>

#include "size_units.h"
#include "stream_utils.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::heap
{
    class ust_address_stack_trace;
    class page_range_descriptor;
    class segment_heap;
    class heap_page_segment;

    class heap_segment_context
    {
    public:
        heap_segment_context(segment_heap const& heap, uint64_t heap_segment_context_address);

        [[nodiscard]] segment_heap const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_segment_context_address() const { return heap_segment_context_address_; }

        [[nodiscard]] size_units::base_16::bytes max_allocation_size() const;
        [[nodiscard]] uint64_t segment_count() const;
        [[nodiscard]] uint64_t segment_mask() const;
        [[nodiscard]] uint8_t unit_shift() const;
        [[nodiscard]] uint8_t pages_per_unit_shift() const;

        [[nodiscard]] uint64_t heap_key() const;
        [[nodiscard]] ust_address_stack_trace const& stack_trace() const;

        [[nodiscard]] std::generator<heap_page_segment> pages() const;
        [[nodiscard]] std::generator<page_range_descriptor> free_page_ranges() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_segment_context_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_seg_context_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(segment_heap const& heap);

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_seg_context_symbol_type;

            stream_utils::symbol_type_and_base_type_field_offset segment_heap_seg_context_max_allocation_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_seg_context_segment_count_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_seg_context_segment_mask_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_seg_context_unit_shift_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_seg_context_pages_per_unit_shift_field_data;

            dbg_help::symbol_type_and_field_offset segment_heap_seg_context_segment_list_head_field_data;
            dbg_help::symbol_type_and_field_offset segment_heap_seg_context_free_page_ranges_field_data;
        };

        cache_data const* cache_data_;
        segment_heap const* heap_;
        uint64_t heap_segment_context_address_;
    };
}
