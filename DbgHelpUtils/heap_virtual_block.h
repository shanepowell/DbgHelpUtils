#pragma once
#include <cstdint>

#include "size_units.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::heap
{
    class nt_heap;
    class heap_entry;

    class heap_virtual_block
    {
    public:
        heap_virtual_block(nt_heap const& heap, uint64_t heap_virtual_block_address);

        [[nodiscard]] uint64_t descriptor_address() const { return heap_virtual_block_address_; }
        [[nodiscard]] uint64_t address() const { return data_address_; }
        [[nodiscard]] size_units::base_10::bytes reserved() const;
        [[nodiscard]] size_units::base_10::bytes committed() const;

        [[nodiscard]] std::experimental::generator<heap_entry> entries() const;

    private:
        [[nodiscard]] uint64_t get_machine_size_field_value(std::wstring const& field_name) const;

    private:
        nt_heap const& heap_;
        uint64_t const heap_virtual_block_address_;
        dbg_help::symbol_type_info const heap_virtual_block_symbol_type_;
        uint64_t const data_address_;
        uint64_t const busy_block_offset_;
    };
}
