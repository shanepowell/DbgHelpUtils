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

    class rtl_balanced_links_walker
    {
    public:
        rtl_balanced_links_walker(cache_manager& cache, stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t head_node_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name);
        rtl_balanced_links_walker(cache_manager& cache, stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t head_node_address);

        [[nodiscard]] std::experimental::generator<uint64_t> entries() const;

        static std::wstring const& symbol_name;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info rtl_balanced_links_symbol_type;
            std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> right_field_data;
            std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> left_field_data;
        };

        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager& cache_manager_;
        stream_stack_dump::mini_dump_stack_walk const& walker_;
        uint64_t const head_node_address_;
        uint64_t const rtl_balanced_link_entry_offset_;
        cache_data const& cache_data_{setup_globals()};
    };

}
