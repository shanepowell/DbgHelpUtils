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
    class heap_vs_entry;
    class segment_heap;
    class heap_vs_subsegment;

    class heap_vs_context
    {
    public:
        heap_vs_context(segment_heap const& heap, uint64_t heap_vs_context_address);

        [[nodiscard]] segment_heap const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_vs_context_address() const { return heap_vs_context_address_; }

        [[nodiscard]] uint64_t total_committed_units() const;
        [[nodiscard]] uint64_t free_committed_units() const;

        [[nodiscard]] std::generator<heap_vs_subsegment> subsegments() const;
        [[nodiscard]] std::generator<heap_vs_entry> free_entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_vs_context_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_vs_context_symbol_type; }

        static std::wstring const& symbol_name;
        static std::wstring const& free_chunk_symbol_name;
        static void setup_globals(segment_heap const& heap);

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_vs_context_symbol_type;
            dbg_help::symbol_type_info heap_vs_chunk_free_symbol_type;
            size_t heap_vs_chunk_header_length{};

            stream_utils::symbol_type_and_base_type_field_offset heap_vs_context_total_committed_units_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_vs_context_free_committed_units_field_data;

            dbg_help::symbol_type_and_field_offset heap_vs_context_subsegment_list_field_data;
            dbg_help::symbol_type_and_field_offset heap_vs_context_free_chunk_tree_field_data;
        };

        cache_data const* cache_data_;
        segment_heap const* heap_;
        uint64_t heap_vs_context_address_;
    };
}
