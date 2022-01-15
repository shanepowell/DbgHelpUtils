#pragma once
#include <cstdint>

#include "page_range_flags_utils.h"
#include "size_units.h"
#include "stream_utils.h"
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
    class heap_segment_context;

    class page_range_descriptor
    {
    public:
        page_range_descriptor(heap_segment_context const& heap, uint64_t page_range_descriptor_address, size_t index, uint64_t heap_page_segment_address);

        [[nodiscard]] heap_segment_context const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t page_range_descriptor_address() const { return page_range_descriptor_address_; }
        [[nodiscard]] size_t index() const { return index_; }

        [[nodiscard]] bool extra_present() const;
        [[nodiscard]] size_units::base_16::bytes extra_bytes() const;
        [[nodiscard]] page_range_flags_utils::page_range_flags range_flags() const;
        [[nodiscard]] uint8_t committed_page_count() const;
        [[nodiscard]] uint8_t unit_offset() const;
        [[nodiscard]] size_units::base_16::bytes unit_size() const;
        [[nodiscard]] uint64_t block_address() const;
        [[nodiscard]] size_units::base_16::bytes block_size() const;
        [[nodiscard]] uint64_t user_address() const;
        [[nodiscard]] size_units::base_16::bytes user_requested_size() const;

        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }
        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }

        [[nodiscard]] bool is_start_of_range() const;

        [[nodiscard]] uint64_t symbol_address() const { return page_range_descriptor_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_.heap_page_range_descriptor_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(segment_heap const& heap);

    private:
        [[nodiscard]] int64_t read_front_padding_size() const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_page_range_descriptor_symbol_type;
            stream_utils::symbol_type_and_base_type_field_offset heap_page_range_descriptor_unused_bytes_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_page_range_descriptor_range_flags_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_page_range_descriptor_committed_page_count_field_data_x64;
            stream_utils::symbol_type_and_base_type_field_offset heap_page_range_descriptor_unit_size_field_data;

            dbg_help::symbol_type_and_field_offset heap_page_range_descriptor_extra_present_field_data;
            dbg_help::symbol_type_and_field_offset heap_page_range_descriptor_range_flags_bits_field_data;
            dbg_help::symbol_type_and_field_offset heap_page_range_descriptor_committed_page_count_field_data_x86;
        };

        cache_data const& cache_data_;
        heap_segment_context const& heap_;
        uint64_t const page_range_descriptor_address_;
        size_t const index_;
        uint64_t const heap_page_segment_address_;
        uint8_t unit_shift_;
        uint64_t const ust_address_{0};
        std::vector<uint64_t> const allocation_stack_trace_{};
    };
}

