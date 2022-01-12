#pragma once
#include <cstdint>
#include <experimental/generator>

#include "symbol_type_info.h"

namespace dlg_help_utils
{
    class cache_manager;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::ntdll_utilities
{

    class rtl_rb_tree_walker
    {
    public:
        rtl_rb_tree_walker(cache_manager& cache, stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t rb_tree_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name);

        [[nodiscard]] std::experimental::generator<uint64_t> entries() const;

        static std::wstring const& symbol_name;
        static std::wstring const& node_symbol_name;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info rtl_rb_tree_symbol_type;
            dbg_help::symbol_type_info rtl_rb_balanced_node_symbol_type;
            std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> root_field_data;
            std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> right_field_data;
            std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> left_field_data;
        };

        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager& cache_manager_;
        stream_stack_dump::mini_dump_stack_walk const& walker_;
        uint64_t const rb_tree_address_;
        uint64_t rtl_balanced_node_entry_offset_;
        cache_data const& cache_data_{setup_globals()};
    };

}
