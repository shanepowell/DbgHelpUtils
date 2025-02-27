#pragma once
#include <cstdint>
#include "generator.h"

#include "stream_utils.h"
#include "symbol_type_info.h"

namespace dlg_help_utils
{
    class cache_manager;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::ntdll_utilities
{

    class rtl_balanced_links_walker
    {
    public:
        rtl_balanced_links_walker(cache_manager& cache, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t head_node_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name);
        rtl_balanced_links_walker(cache_manager& cache, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t head_node_address);

        [[nodiscard]] cache_manager& cache() const { return *cache_manager_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const { return *walker_; }

        [[nodiscard]] dlg_help_utils::generator<uint64_t> entries() const;

        static std::wstring const& symbol_name;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info rtl_balanced_links_symbol_type;
            stream_utils::symbol_type_and_base_type_field_offset right_field_data;
            stream_utils::symbol_type_and_base_type_field_offset left_field_data;
        };

        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager* cache_manager_;
        stream_stack_dump::mini_dump_memory_walker const* walker_;
        uint64_t head_node_address_;
        uint64_t rtl_balanced_link_entry_offset_;
        cache_data const* cache_data_{&setup_globals()};
    };

}
