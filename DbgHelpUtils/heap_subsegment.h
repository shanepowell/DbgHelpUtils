#pragma once
#include <cstdint>

#include "symbol_type_info.h"

namespace dlg_help_utils::heap
{
    class heap_entry;
    class lfh_heap;

    class heap_subsegment
    {
    public:
        heap_subsegment(lfh_heap const& heap, uint64_t heap_subsegment_address, uint64_t lfh_block_zone_size);

        [[nodiscard]] uint64_t address() const { return heap_subsegment_address_; }
        [[nodiscard]] uint16_t block_size() const;
        [[nodiscard]] uint16_t block_size_raw() const;
        [[nodiscard]] uint16_t block_count() const;

        [[nodiscard]] uint64_t entry_start_address() const { return entry_start_address_; }
        [[nodiscard]] uint16_t block_stride() const { return block_stride_; }

        [[nodiscard]] std::experimental::generator<heap_entry> entries() const;

    private:
        [[nodiscard]] std::tuple<uint64_t, uint16_t> get_entry_data() const;

        template<typename T>
        [[nodiscard]] T get_field_value(std::wstring const& field_name) const;

        [[noreturn]] static void throw_cant_get_field_data(std::wstring const& field_name);
        [[noreturn]] static void throw_cant_get_user_data_field_data(std::wstring const& field_name);

    private:
        lfh_heap const& lfh_heap_;
        uint64_t const heap_subsegment_address_;
        uint64_t const lfh_block_zone_size_;
        dbg_help::symbol_type_info const heap_subsegment_symbol_type_;
        dbg_help::symbol_type_info const heap_user_data_header_symbol_type_;
        uint64_t entry_start_address_;
        uint16_t block_stride_;
    };
}
