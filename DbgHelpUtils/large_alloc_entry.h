#pragma once

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

    class large_alloc_entry
    {
    public:
        large_alloc_entry(segment_heap const& heap, uint64_t large_alloc_entry_address);

        [[nodiscard]] segment_heap const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t large_alloc_entry_address() const { return large_alloc_entry_address_; }

        [[nodiscard]] size_units::base_16::bytes size() const { return size_; }
        [[nodiscard]] uint64_t virtual_address() const;
        [[nodiscard]] size_units::base_16::bytes unused_bytes() const;
        [[nodiscard]] bool extra_present() const;
        [[nodiscard]] uint32_t guard_page_count() const;
        [[nodiscard]] uint32_t guard_page_alignment() const;
        [[nodiscard]] uint32_t spare() const;
        [[nodiscard]] uint64_t allocated_pages() const;

        [[nodiscard]] uint64_t block_address() const;
        [[nodiscard]] uint64_t block_size() const;
        [[nodiscard]] uint64_t user_address() const;
        [[nodiscard]] size_units::base_16::bytes user_requested_size() const;

        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }
        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }

        [[nodiscard]] uint64_t symbol_address() const { return large_alloc_entry_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_large_alloc_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(segment_heap const& heap);

    private:
        [[nodiscard]] size_units::base_16::bytes get_size() const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_large_alloc_symbol_type;
            size_t heap_large_alloc_length{};

            stream_utils::symbol_type_and_base_type_field_offset heap_large_alloc_virtual_address_field_data;

            dbg_help::symbol_type_and_field_offset heap_large_alloc_unused_bytes_field_data;
            dbg_help::symbol_type_and_field_offset heap_large_alloc_extra_present_field_data;
            dbg_help::symbol_type_and_field_offset heap_large_alloc_guard_page_count_field_data;
            dbg_help::symbol_type_and_field_offset heap_large_alloc_guard_page_alignment_field_data;
            dbg_help::symbol_type_and_field_offset heap_large_alloc_spare_field_data;
            dbg_help::symbol_type_and_field_offset heap_large_alloc_allocated_pages_field_data;
        };

        cache_data const* cache_data_;
        segment_heap const* heap_;
        uint64_t large_alloc_entry_address_;
        size_units::base_16::bytes size_;
        uint64_t ust_address_{0};
        std::vector<uint64_t> allocation_stack_trace_{};
   };
}
