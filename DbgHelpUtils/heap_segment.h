#pragma once
#include <cstdint>
#include <vector>
#include <experimental/generator>

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
    class nt_heap;
    class heap_entry;
    class heap_ucr_descriptor;

    constexpr uint32_t SegmentSignatureNtHeap = 0xffeeffee;
    constexpr uint32_t SegmentSignatureSegmentHeap = 0xddeeddee;

    class heap_segment
    {
    public:
        heap_segment(nt_heap const& heap, uint64_t heap_segment_address);

        [[nodiscard]] nt_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;
        [[nodiscard]] uint64_t heap_segment_address() const { return heap_segment_address_; }

        [[nodiscard]] uint32_t segment_flags() const;
        [[nodiscard]] uint64_t base_address() const;
        [[nodiscard]] uint32_t number_of_pages() const;
        [[nodiscard]] uint64_t first_entry() const;
        [[nodiscard]] uint64_t last_entry() const;
        [[nodiscard]] uint32_t number_of_uncommitted_pages() const;
        [[nodiscard]] uint32_t number_of_uncommitted_ranges() const;
        [[nodiscard]] uint16_t segment_allocator_back_trace_index() const;

        [[nodiscard]] std::experimental::generator<heap_entry> entries() const;

        [[nodiscard]] std::experimental::generator<heap_ucr_descriptor> uncommitted_ranges() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_segment_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_segment_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] static std::optional<heap_ucr_descriptor> is_uncommitted_range(std::vector<heap_ucr_descriptor> const& ranges, uint64_t heap_entry_address);

    private:
        nt_heap const& heap_;
        uint64_t const heap_segment_address_;
        dbg_help::symbol_type_info const heap_segment_symbol_type_;
    };
}
