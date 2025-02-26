#pragma once
#include <cstdint>
#include <functional>
#include <generator>

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

    class list_entry_walker
    {
    public:
        list_entry_walker(cache_manager& cache, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t start_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name, std::function<uint64_t (uint64_t, uint64_t)> address_decoder = {});

        [[nodiscard]] cache_manager& cache() const { return *cache_manager_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const { return *walker_; }

        [[nodiscard]] std::generator<uint64_t> entries() const;

        static std::wstring const& symbol_name;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info list_entry_symbol_type;
            stream_utils::symbol_type_and_base_type_field_offset flink_field_data;
        };

        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager* cache_manager_;
        stream_stack_dump::mini_dump_memory_walker const* walker_;
        uint64_t start_address_;
        std::function<uint64_t (uint64_t, uint64_t)> address_decoder_;
        uint64_t list_entry_entry_offset_;
        cache_data const* cache_data_{&setup_globals()};
    };

}
