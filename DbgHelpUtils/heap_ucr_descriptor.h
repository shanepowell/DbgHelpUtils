#pragma once
#include <cstdint>

#include "size_units.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::heap
{
    class nt_heap;

    class heap_ucr_descriptor
    {
    public:
        heap_ucr_descriptor(nt_heap const& heap, uint64_t heap_ucr_descriptor_address);

        [[nodiscard]] uint64_t address() const;
        [[nodiscard]] size_units::base_10::bytes size() const;

    private:
        [[nodiscard]] uint64_t get_machine_size_field_value(std::wstring const& field_name) const;

        [[noreturn]] static void throw_cant_get_field_data(std::wstring const& field_name);

    private:
        nt_heap const& heap_;
        uint64_t const heap_ucr_descriptor_address_;
        dbg_help::symbol_type_info const heap_ucr_descriptor_symbol_type_;
    };
}
