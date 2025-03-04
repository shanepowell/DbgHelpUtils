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
    class heap_lfh_bucket;
    class segment_heap;

    class heap_lfh_context
    {
    public:
        heap_lfh_context(segment_heap const& heap, uint64_t heap_lfh_context_address);

        [[nodiscard]] segment_heap const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;
        [[nodiscard]] ust_address_stack_trace const& stack_trace() const;

        [[nodiscard]] uint64_t heap_lfh_context_address() const { return heap_lfh_context_address_; }

        [[nodiscard]] uint8_t max_affinity() const;
        [[nodiscard]] size_units::base_16::bytes max_block_size() const;
        [[nodiscard]] bool with_old_page_crossing_blocks() const;
        [[nodiscard]] bool disable_randomization() const;

        [[nodiscard]] generator<heap_lfh_bucket> active_buckets() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_lfh_context_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_lfh_context_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(segment_heap const& heap);

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_lfh_context_symbol_type;
            dbg_help::symbol_type_info heap_lfh_context_buckets_field_symbol_type;
            uint64_t heap_lfh_context_buckets_field_offset{};

            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_context_max_affinity_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_context_config_max_block_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_context_config_with_old_page_crossing_blocks_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_context_config_disable_randomization_field_data;
        };

    private:
        cache_data const* cache_data_;
        segment_heap const* heap_;
        uint64_t heap_lfh_context_address_;
    };
}
