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
    class heap_vs_entry;
    class segment_heap;
    class heap_vs_subsegment;

    class heap_vs_context
    {
    public:
        heap_vs_context(segment_heap const& heap, uint64_t heap_vs_context_address);

        [[nodiscard]] segment_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_vs_context_address() const { return heap_vs_context_address_; }

        [[nodiscard]] uint64_t total_committed_units() const;
        [[nodiscard]] uint64_t free_committed_units() const;

        [[nodiscard]] std::experimental::generator<heap_vs_subsegment> subsegments() const;
        [[nodiscard]] std::experimental::generator<heap_vs_entry> free_entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_vs_context_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_vs_context_symbol_type_; }

        static std::wstring const& symbol_name;
        static std::wstring const& free_chunk_symbol_name;

    private:
        segment_heap const& heap_;
        uint64_t const heap_vs_context_address_;
        dbg_help::symbol_type_info const heap_vs_context_symbol_type_;
        dbg_help::symbol_type_info const heap_vs_chunk_free_symbol_type_;
        uint64_t const heap_vs_chunk_header_length_;
    };
}
