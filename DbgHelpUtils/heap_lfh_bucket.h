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
    class heap_lfh_affinity_slot;
    class heap_lfh_context;

    class heap_lfh_bucket
    {
    public:
        heap_lfh_bucket(heap_lfh_context const& heap, uint64_t heap_lfh_bucket_address, int16_t bucket_index);

        [[nodiscard]] heap_lfh_context const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] bool is_enabled() const { return is_enabled_; }
        [[nodiscard]] uint16_t usage_count() const;
        [[nodiscard]] size_t bucket_index() const { return bucket_index_; }
        [[nodiscard]] size_units::base_10::bytes bucket_granularity() const { return bucket_granularity_; }
        [[nodiscard]] size_units::base_10::bytes max_allocation_size() const { return max_allocation_size_; }
        [[nodiscard]] uint64_t heap_lfh_bucket_address() const { return heap_lfh_bucket_address_; }

        [[nodiscard]] size_t slot_count() const;
        [[nodiscard]] uint64_t total_block_count() const;
        [[nodiscard]] uint64_t total_subsegment_count() const;
        [[nodiscard]] size_units::base_10::bytes reciprocal_block_size() const;
        [[nodiscard]] uint8_t shift() const;
        [[nodiscard]] std::vector<uint8_t> processor_affinity_mapping() const;

        [[nodiscard]] std::experimental::generator<heap_lfh_affinity_slot> affinity_slots() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_lfh_bucket_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_lfh_bucket_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        void validate_enabled() const;
        [[nodiscard]] size_t get_bucket_index() const;
        [[nodiscard]] size_units::base_10::bytes get_bucket_granularity() const;
        [[nodiscard]] size_units::base_10::bytes get_max_allocation_size() const;

    private:
        heap_lfh_context const& heap_;
        uint64_t const heap_lfh_bucket_address_;
        bool const is_enabled_;
        dbg_help::symbol_type_info const heap_lfh_bucket_symbol_type_;
        dbg_help::symbol_type_info const heap_lfh_ondemand_pointer_symbol_type_;
        size_t const bucket_index_;
        size_units::base_10::bytes const bucket_granularity_;
        size_units::base_10::bytes const max_allocation_size_;
    };
}
