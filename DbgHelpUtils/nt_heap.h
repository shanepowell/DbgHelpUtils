#pragma once
#include <cstdint>
#include <experimental/generator>

#include "gflags_utils.h"
#include "size_units.h"
#include "symbol_type_info.h"

namespace dlg_help_utils
{
    namespace stream_stack_dump
    {
        class mini_dump_stack_walk;
    }
}

namespace dlg_help_utils::heap
{
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

        nt_heap(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t nt_heap_address, gflags_utils::gflags nt_global_flag);

        [[nodiscard]] uint64_t nt_heap_address() const { return nt_heap_address_; }
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
        [[nodiscard]] gflags_utils::gflags nt_global_flag() const { return nt_global_flag_; }

        [[nodiscard]] std::experimental::generator<heap_segment> segments() const;
        [[nodiscard]] std::experimental::generator<heap_ucr_descriptor> uncommitted_ranges() const;
        [[nodiscard]] std::experimental::generator<heap_virtual_block> heap_virtual_blocks() const;
        [[nodiscard]] std::experimental::generator<heap_entry> free_entries() const;

        [[nodiscard]] std::optional<lfh_heap> lfh_heap() const;

        [[nodiscard]] bool heap_page_alloc_enabled() const;
        [[nodiscard]] bool user_stack_db_enabled() const;

        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const { return walker_; }

        bool decode_heap_entry(void const* src, void* dst) const;

        [[nodiscard]] bool is_x86_target() const { return granularity() == 8; }
        [[nodiscard]] bool is_x64_target() const { return granularity() == 16; }

    private:
        [[nodiscard]] uint64_t get_field_pointer(std::wstring const& field_name) const;
        [[nodiscard]] uint64_t get_field_address(std::wstring const& field_name) const;

        template<typename T>
        [[nodiscard]] T get_field_value(std::wstring const& field_name) const;
        [[nodiscard]] uint64_t get_machine_size_field_value(std::wstring const& field_name) const;

        [[nodiscard]] static uint64_t get_segment_entry_offset(dbg_help::symbol_type_info const& heap_symbol_type);
        [[nodiscard]] static uint64_t get_free_entry_free_list_offset(dbg_help::symbol_type_info const& heap_free_entry_symbol_type);
        [[nodiscard]] static uint32_t get_granularity(stream_stack_dump::mini_dump_stack_walk const& walker);
        [[noreturn]] static void throw_cant_get_field_data(std::wstring const& field_name);
        [[noreturn]] static void throw_cant_get_field_is_null(std::wstring const& field_name);

    private:
        stream_stack_dump::mini_dump_stack_walk const& walker_;
        uint64_t const nt_heap_address_;
        gflags_utils::gflags const nt_global_flag_;
        dbg_help::symbol_type_info const heap_symbol_type_;
        dbg_help::symbol_type_info const list_entry_symbol_type_;
        dbg_help::symbol_type_info const heap_free_entry_symbol_type_;
        uint32_t const granularity_;
        uint64_t const segment_entry_offset_;
        uint64_t const free_entry_free_list_offset_;
        std::unique_ptr<char[]> encoding_;
    };
}
