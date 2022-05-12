﻿#pragma once

#include <cstdint>
#include <string>

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
    class segment_heap;

    class heap_vs_entry
    {
    public:
        heap_vs_entry(segment_heap const& heap, uint64_t heap_vs_entry_address, std::unique_ptr<uint8_t[]> buffer, uint16_t previous_size);
        heap_vs_entry(segment_heap const& heap, uint64_t heap_vs_entry_address, std::unique_ptr<uint8_t[]> buffer);
        heap_vs_entry(segment_heap const& heap, uint64_t heap_vs_entry_address, uint64_t uncommitted_size);

        [[nodiscard]] segment_heap const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_vs_entry_address() const { return heap_vs_entry_address_; }

        [[nodiscard]] bool is_valid() const { return is_valid_; }

        [[nodiscard]] size_units::base_16::bytes size() const { return size_; }
        [[nodiscard]] size_units::base_16::bytes previous_size() const { return previous_size_; }
        [[nodiscard]] bool uncommitted_range() const { return uncommitted_range_; }
        [[nodiscard]] uint16_t memory_cost() const;
        [[nodiscard]] bool allocated() const;
        [[nodiscard]] uint8_t segment_page_offset() const;
        [[nodiscard]] bool has_unused_bytes() const;
        [[nodiscard]] size_units::base_16::bytes unused_bytes() const;
        [[nodiscard]] bool skip_during_walk() const;
        [[nodiscard]] uint32_t spare() const;
        [[nodiscard]] uint32_t allocated_chunk_bits() const;

        [[nodiscard]] uint64_t block_address() const;
        [[nodiscard]] uint64_t block_size() const;
        [[nodiscard]] uint64_t user_address() const;
        [[nodiscard]] size_units::base_16::bytes user_requested_size() const;

        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }
        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }

        [[nodiscard]] uint16_t raw_size() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_vs_entry_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_vs_chunk_header_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(segment_heap const& heap);

    private:
        [[nodiscard]] size_units::base_16::bytes get_size() const;
        [[nodiscard]] size_units::base_16::bytes get_previous_size() const;
        [[nodiscard]] uint16_t get_previous_size_raw() const;
        [[nodiscard]] bool get_is_valid(uint16_t previous_size) const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;
        void validate_buffer() const;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_vs_chunk_header_symbol_type;
            size_t heap_vs_chunk_header_length{};

            stream_utils::symbol_type_and_base_type_field_offset heap_vs_chunk_header_allocated_chunk_bits_field_data;

            dbg_help::symbol_type_and_field_offset heap_vs_chunk_header_sizes_memory_cost_field_data;
            dbg_help::symbol_type_and_field_offset heap_vs_chunk_header_sizes_allocated_field_data;
            dbg_help::symbol_type_and_field_offset heap_vs_chunk_header_encoded_segment_page_offset_field_data;
            dbg_help::symbol_type_and_field_offset heap_vs_chunk_header_unused_bytes_field_data;
            dbg_help::symbol_type_and_field_offset heap_vs_chunk_header_skip_during_walk_field_data;
            dbg_help::symbol_type_and_field_offset heap_vs_chunk_header_spare_field_data;
            dbg_help::symbol_type_and_field_offset heap_vs_chunk_header_sizes_unsafe_size_field_data;
            dbg_help::symbol_type_and_field_offset heap_vs_chunk_header_sizes_unsafe_prev_size_field_data;
        };

        cache_data const* cache_data_;
        segment_heap const* heap_;
        uint64_t heap_vs_entry_address_;
        std::unique_ptr<uint8_t[]> buffer_;
        size_units::base_16::bytes size_;
        size_units::base_16::bytes previous_size_{0};
        bool is_valid_{true};
        bool uncommitted_range_{false};
        uint64_t ust_address_{0};
        std::vector<uint64_t> allocation_stack_trace_{};
   };
}
