#pragma once
#include <cstdint>
#include <vector>
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

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::heap
{
    class crt_entry;

    class crt_heap
    {
    public:
        crt_heap(cache_manager& cache, process::process_environment_block const& peb);

        void set_base_diff_filter(crt_heap& base_diff_filter);
        void clear_base_diff_filter();

        [[nodiscard]] cache_manager& cache() const { return cache_manager_; }
        [[nodiscard]] process::process_environment_block const& peb() const { return peb_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;

        [[nodiscard]] bool is_using_crt_heap() const { return crt_first_block_ != 0; }

        [[nodiscard]] std::experimental::generator<crt_entry> entries() const;

    private:
        [[nodiscard]] uint64_t get_crt_first_block() const;

        [[nodiscard]] std::experimental::generator<crt_entry> all_entries() const;
        [[nodiscard]] std::experimental::generator<crt_entry> filter_entries() const;

        [[nodiscard]] static bool is_filtered(std::vector<crt_entry> const& filters, crt_entry const& entry);

        struct cache_data
        {
            dbg_help::symbol_type_info block_symbol_symbol_type;
            std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> crt_mem_block_header_block_header_next_field_data;
        };

        [[nodiscard]] cache_data const& setup_globals();

    private:
        cache_manager& cache_manager_;
        process::process_environment_block const& peb_;
        uint64_t const crt_first_block_;
        cache_data const& cache_data_{setup_globals()};
        crt_heap* base_diff_filter_{nullptr};
    };
}
