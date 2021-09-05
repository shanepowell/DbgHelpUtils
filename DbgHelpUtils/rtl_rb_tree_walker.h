#pragma once
#include <cstdint>
#include <experimental/generator>

#include "symbol_type_info.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::ntdll_utilities
{

    class rtl_rb_tree_walker
    {
    public:
        rtl_rb_tree_walker(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t rb_tree_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name);

        [[nodiscard]] std::experimental::generator<uint64_t> entries() const;

        static std::wstring const& symbol_name;
        static std::wstring const& node_symbol_name;

    private:
        stream_stack_dump::mini_dump_stack_walk const& walker_;
        uint64_t const rb_tree_address_;
        dbg_help::symbol_type_info const rtl_rb_tree_symbol_type_;
        dbg_help::symbol_type_info const rtl_rb_balanced_node_symbol_type_;
        uint64_t const rtl_balanced_node_entry_offset_;
    };

}
