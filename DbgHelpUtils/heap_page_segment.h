#pragma once
#include <cstdint>

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
    class segment_heap;
    class heap_segment_context;
    class page_range_descriptor;

    class heap_page_segment
    {
    public:
        heap_page_segment(heap_segment_context const& heap, uint64_t heap_page_segment_address, uint64_t heap_segment_context_address);

        [[nodiscard]] heap_segment_context const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_page_segment_address() const { return heap_page_segment_address_; }
        [[nodiscard]] uint64_t signature() const;
        [[nodiscard]] bool is_signature_valid() const;

        [[nodiscard]] std::experimental::generator<page_range_descriptor> all_entries() const;
        [[nodiscard]] std::experimental::generator<page_range_descriptor> entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_page_segment_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_.heap_page_segment_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(segment_heap const& heap);

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_page_segment_symbol_type;
            size_t heap_page_range_descriptor_length{};
            dbg_help::symbol_type_info desc_array_array_field_symbol_type;
            uint64_t heap_seg_context_array_field_offset{};

            stream_utils::symbol_type_and_base_type_field_offset heap_page_segment_signature_field_data;
        };

    private:
        cache_data const& cache_data_;
        heap_segment_context const& heap_;
        uint64_t const heap_page_segment_address_;
        uint64_t const heap_segment_context_address_;
    };
}
