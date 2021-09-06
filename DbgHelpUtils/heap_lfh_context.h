#pragma once
#include <cstdint>

#include "size_units.h"
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
    class ust_address_stack_trace;
    class heap_lfh_bucket;
    class segment_heap;

    class heap_lfh_context
    {
    public:
        heap_lfh_context(segment_heap const& heap, uint64_t heap_lfh_context_address);

        [[nodiscard]] segment_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;
        [[nodiscard]] ust_address_stack_trace const& stack_trace() const;

        [[nodiscard]] uint64_t heap_lfh_context_address() const { return heap_lfh_context_address_; }

        [[nodiscard]] uint8_t max_affinity() const;
        [[nodiscard]] size_units::base_10::bytes max_block_size() const;
        [[nodiscard]] bool with_old_page_crossing_blocks() const;
        [[nodiscard]] bool disable_randomization() const;

        [[nodiscard]] std::experimental::generator<heap_lfh_bucket> active_buckets() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_lfh_context_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_lfh_context_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] std::pair<dbg_help::symbol_type_info, uint64_t> get_heap_lfh_context_buckets_array_data() const;
        [[nodiscard]] dbg_help::symbol_type_info get_heap_lfh_context_buckets_array_field_symbol_type() const;
        [[nodiscard]] uint64_t get_heap_lfh_context_buckets_array_field_offset() const;

    private:
        segment_heap const& heap_;
        uint64_t const heap_lfh_context_address_;
        dbg_help::symbol_type_info const heap_lfh_context_symbol_type_;
        dbg_help::symbol_type_info const heap_lfh_context_buckets_field_symbol_type_;
        uint64_t const heap_lfh_context_buckets_field_offset_;
    };
}
