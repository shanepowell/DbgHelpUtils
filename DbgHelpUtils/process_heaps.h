#pragma once
#include "process_environment_block.h"
#include "size_units.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace dlg_help_utils::dbg_help
{
    class symbol_engine;
}

namespace dlg_help_utils::heap
{
    namespace statistic_views
    {
        class statistic_view_options;
        class system_module_list;
    }

    class heap_subsegment;
    class heap_entry;
    class process_heap_entry;
    class process_heaps_statistics;

    class process_heaps
    {
    public:
        process_heaps(mini_dump const& mini_dump, cache_manager& cache, dbg_help::symbol_engine& symbol_engine, statistic_views::system_module_list const& system_module_list, statistic_views::statistic_view_options const& statistic_view_options);

        [[nodiscard]] process::process_environment_block const& peb() const { return peb_; }

        void set_base_diff_filter(process_heaps& base_diff_filter);
        void clear_base_diff_filter();

        [[nodiscard]] std::experimental::generator<process_heap_entry> entries() const;
        [[nodiscard]] std::experimental::generator<process_heap_entry> free_entries() const;

        [[nodiscard]] bool is_address_filtered(uint64_t address, size_units::base_16::bytes size) const;

        [[nodiscard]] process_heaps_statistics statistics() const;

        [[nodiscard]] static bool is_lfh_subsegment_in_entry(heap_entry const& entry, heap_subsegment const& subsegment);

    private:
        void clear_cache() const;

        [[nodiscard]] std::experimental::generator<process_heap_entry> all_entries() const;
        [[nodiscard]] std::experimental::generator<process_heap_entry> all_free_entries() const;

        [[nodiscard]] std::experimental::generator<process_heap_entry> filter_entries() const;
        [[nodiscard]] std::experimental::generator<process_heap_entry> filter_free_entries() const;

        [[nodiscard]] static bool is_filtered(std::vector<process_heap_entry> const& filters, process_heap_entry const& entry);
        [[nodiscard]] static bool is_address_filtered(std::vector<process_heap_entry> const& filters, uint64_t address, size_units::base_16::bytes size);
        [[nodiscard]] static bool contains_address(uint64_t start_address, uint64_t size, uint64_t address);

    private:
        cache_manager& cache_manager_;
        process::process_environment_block const peb_;
        statistic_views::system_module_list const& system_module_list_;
        statistic_views::statistic_view_options const& statistic_view_options_;
        process_heaps* base_diff_filter_{nullptr};
        mutable std::vector<process_heap_entry> entry_filters_cache_;
        mutable std::vector<process_heap_entry> free_entry_filters_cache_;
        mutable std::vector<process_heap_entry> entry_cache_;
        mutable std::vector<process_heap_entry> free_entry_cache_;
    };
}
