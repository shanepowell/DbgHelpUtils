#pragma once
#include <cstdint>
#include <vector>
#include <experimental/generator>

#include "symbol_type_info.h"

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

    private:
        [[nodiscard]] static std::optional<heap_ucr_descriptor> is_uncommitted_range(std::vector<heap_ucr_descriptor> const& ranges, uint64_t heap_entry_address);

        [[nodiscard]] uint64_t get_field_pointer(std::wstring const& field_name) const;
        [[nodiscard]] uint64_t get_field_address(std::wstring const& field_name) const;

        template<typename T>
        [[nodiscard]] T get_field_value(std::wstring const& field_name) const;

        [[nodiscard]] static uint64_t get_segment_entry_offset(dbg_help::symbol_type_info const& heap_ucr_descriptor_symbol_type);
        [[noreturn]] static void throw_cant_get_field_data(std::wstring const& field_name);

    private:
        nt_heap const& heap_;
        uint64_t const heap_segment_address_;
        dbg_help::symbol_type_info const heap_segment_symbol_type_;
        dbg_help::symbol_type_info const heap_entry_symbol_type_;
        dbg_help::symbol_type_info const heap_ucr_descriptor_symbol_type_;
        dbg_help::symbol_type_info const list_entry_symbol_type_;
        uint64_t const segment_entry_offset_;
    };
}
