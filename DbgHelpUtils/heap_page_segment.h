#pragma once
#include <cstdint>

#include "symbol_type_info.h"

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::heap
{
    class segment_heap;
    class page_range_descriptor;

    class heap_page_segment
    {
    public:
        heap_page_segment(segment_heap const& heap, uint64_t heap_page_segment_address, uint64_t heap_segment_context_address);

        [[nodiscard]] segment_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_page_segment_address() const { return heap_page_segment_address_; }
        [[nodiscard]] uint64_t signature() const;
        [[nodiscard]] bool is_signature_valid() const;

        [[nodiscard]] std::experimental::generator<page_range_descriptor> all_entries() const;
        [[nodiscard]] std::experimental::generator<page_range_descriptor> entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_page_segment_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_page_segment_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] std::pair<dbg_help::symbol_type_info, uint64_t> get_desc_array_field_data() const;
        [[nodiscard]] dbg_help::symbol_type_info get_desc_array_field_symbol_type() const;
        [[nodiscard]] uint64_t get_desc_array_field_offset() const;

    private:
        segment_heap const& heap_;
        uint64_t const heap_page_segment_address_;
        uint64_t const heap_segment_context_address_;
        dbg_help::symbol_type_info const heap_page_segment_symbol_type_;
        uint64_t const heap_page_range_descriptor_length_;
        dbg_help::symbol_type_info const desc_array_array_field_symbol_type_{get_desc_array_field_symbol_type()};
        uint64_t const heap_seg_context_array_field_offset_{get_desc_array_field_offset()};
    };
}
