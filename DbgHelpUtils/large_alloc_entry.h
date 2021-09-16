#pragma once

#include <cstdint>
#include <string>

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
    class segment_heap;

    class large_alloc_entry
    {
    public:
        large_alloc_entry(segment_heap const& heap, uint64_t large_alloc_entry_address);

        [[nodiscard]] segment_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
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
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_large_alloc_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] size_units::base_16::bytes get_size() const;
        [[nodiscard]] uint64_t read_front_padding() const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;

    private:
        segment_heap const& heap_;
        uint64_t const large_alloc_entry_address_;
        dbg_help::symbol_type_info const heap_large_alloc_symbol_type_;
        uint64_t const heap_large_alloc_length_;
        size_units::base_16::bytes const size_;
        uint64_t const ust_address_{0};
        std::vector<uint64_t> const allocation_stack_trace_{};
   };
}
