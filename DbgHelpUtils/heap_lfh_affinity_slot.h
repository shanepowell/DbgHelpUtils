﻿#pragma once
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
    class heap_lfh_context;
    class heap_lfh_subsegment;

    class heap_lfh_affinity_slot
    {
    public:
        heap_lfh_affinity_slot(heap_lfh_context const& heap, uint64_t heap_lfh_affinity_slot_address);

        [[nodiscard]] heap_lfh_context const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_lfh_affinity_slot_address() const { return heap_lfh_affinity_slot_address_; }

        [[nodiscard]] size_t bucket_index() const;
        [[nodiscard]] size_t slot_index() const;
        [[nodiscard]] uint64_t available_subsegment_count() const;

        [[nodiscard]] dlg_help_utils::generator<heap_lfh_subsegment> subsegments() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_lfh_affinity_slot_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_lfh_affinity_slot_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(segment_heap const& heap);

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_lfh_affinity_slot_symbol_type;

            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_affinity_slot_bucket_index_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_affinity_slot_slot_index_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_affinity_slot_available_subsegment_count_field_data;

            dbg_help::symbol_type_and_field_offset heap_lfh_affinity_slot_available_subsegment_list_field_data;
            dbg_help::symbol_type_and_field_offset heap_lfh_affinity_slot_full_subsegment_list_field_data;
        };

        cache_data const* cache_data_;
        heap_lfh_context const* heap_;
        uint64_t heap_lfh_affinity_slot_address_;
    };
}

