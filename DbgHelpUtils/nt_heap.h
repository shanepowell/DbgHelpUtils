#pragma once
#include <cstdint>
#include <experimental/generator>

#include "process_environment_block.h"
#include "size_units.h"
#include "symbol_type_info.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::heap
{
    class dph_heap;
    class lfh_heap;
    class heap_ucr_descriptor;
    class heap_segment;
    class heap_virtual_block;
    class heap_entry;

    constexpr uint32_t HeapEncodeFlagMakeEncodingEnabled = 0x100000;
    constexpr uint8_t FrontEndHeapTypeLowFragmentationHeap = 0x02;

    class nt_heap
    {
    public:

        nt_heap(process::process_environment_block const& peb, uint64_t nt_heap_address);

        [[nodiscard]] process::process_environment_block const& peb() const { return peb_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const { return peb_.walker(); }

        [[nodiscard]] uint64_t nt_heap_address() const { return nt_heap_address_; }
        [[nodiscard]] uint32_t segment_signature() const;
        [[nodiscard]] uint32_t flags() const;
        [[nodiscard]] size_units::base_10::bytes reserved() const;
        [[nodiscard]] size_units::base_10::bytes committed() const;
        [[nodiscard]] size_units::base_10::bytes uncommitted() const;
        [[nodiscard]] size_units::base_10::bytes virtual_blocks() const;
        [[nodiscard]] uint32_t total_segments() const;
        [[nodiscard]] uint32_t total_ucrs() const;
        [[nodiscard]] size_units::base_10::bytes total_free_size() const;
        [[nodiscard]] uint16_t allocator_back_trace_index() const;
        [[nodiscard]] uint8_t front_end_heap_type() const;
        [[nodiscard]] bool is_low_fragment_heap_enabled() const { return front_end_heap_type() == FrontEndHeapTypeLowFragmentationHeap; }

        [[nodiscard]] bool is_encoded() const { return encoding_ != nullptr; }
        [[nodiscard]] uint32_t granularity() const { return granularity_; }

        [[nodiscard]] std::experimental::generator<heap_segment> segments() const;
        [[nodiscard]] std::experimental::generator<heap_ucr_descriptor> uncommitted_ranges() const;
        [[nodiscard]] std::experimental::generator<heap_virtual_block> heap_virtual_blocks() const;
        [[nodiscard]] std::experimental::generator<heap_entry> free_entries() const;
        [[nodiscard]] std::optional<dph_heap> debug_page_heap() const;

        [[nodiscard]] std::optional<lfh_heap> lfh_heap() const;

        [[nodiscard]] bool is_process_heap(uint64_t process_heap_address) const;

        void decode_heap_entry(void const* src, void* dst) const;

        [[nodiscard]] ust_address_stack_trace const& stack_trace() const { return stack_trace_; }

        [[nodiscard]] uint64_t symbol_address() const { return nt_heap_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        uint64_t const nt_heap_address_;
        process::process_environment_block const& peb_;
        dbg_help::symbol_type_info const heap_symbol_type_;
        uint32_t const granularity_;
        std::unique_ptr<uint8_t[]> encoding_;
        ust_address_stack_trace stack_trace_{walker()};
    };
}
