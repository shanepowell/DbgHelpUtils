#pragma once
#include <cstdint>
#include <functional>
#include <experimental/generator>

#include "symbol_type_info.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::ntdll_utilities
{

    class list_entry_walker
    {
    public:
        list_entry_walker(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t start_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name, std::function<uint64_t (uint64_t, uint64_t)> address_decoder = {});

        [[nodiscard]] std::experimental::generator<uint64_t> entries() const;

        static std::wstring const& symbol_name;

    private:
        stream_stack_dump::mini_dump_stack_walk const& walker_;
        uint64_t const start_address_;
        std::function<uint64_t (uint64_t, uint64_t)> address_decoder_;
        dbg_help::symbol_type_info const list_entry_symbol_type_;
        uint64_t const list_entry_entry_offset_;
    };

}
