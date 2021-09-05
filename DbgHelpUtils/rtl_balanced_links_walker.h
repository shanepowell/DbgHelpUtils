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

    class rtl_balanced_links_walker
    {
    public:
        rtl_balanced_links_walker(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t head_node_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name);
        rtl_balanced_links_walker(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t head_node_address);

        [[nodiscard]] std::experimental::generator<uint64_t> entries() const;

        static std::wstring const& symbol_name;

    private:
        stream_stack_dump::mini_dump_stack_walk const& walker_;
        uint64_t const head_node_address_;
        dbg_help::symbol_type_info const rtl_balanced_links_symbol_type_;
        uint64_t const rtl_balanced_link_entry_offset_;
    };

}
