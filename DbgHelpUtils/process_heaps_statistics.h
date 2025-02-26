#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
#include <map>
#include <generator>

#include "allocation_stack_trace_helper.h"

namespace dlg_help_utils::heap
{
    namespace statistic_views
    {
        class statistic_view_options;
    }

    class process_heaps;
    class process_heap_entry;
    class process_heaps_statistic_view;

    class process_heaps_statistics
    {
    public:
        process_heaps_statistics(process_heaps const& process, system_module_list const& system_module_list, statistic_views::statistic_view_options const& statistic_view_options);

        [[nodiscard]] process_heaps_statistic_view view_by_size_frequency() const;
        [[nodiscard]] process_heaps_statistic_view view_by_size_ranges_frequency() const;
        [[nodiscard]] process_heaps_statistic_view view_by_stacktrace_frequency() const;
        [[nodiscard]] process_heaps_statistic_view view_by_application_callsite_frequency() const;

    private:
        [[nodiscard]] std::map<uint64_t, process_heap_entry> get_allocated_entries() const;
        [[nodiscard]] std::map<uint64_t, process_heap_entry> get_free_entries() const;
        [[nodiscard]] static std::map<uint64_t, process_heap_entry> get_all_entries(std::generator<process_heap_entry> entries);

    private:
        process_heaps const& process_;
        statistic_views::statistic_view_options const& statistic_view_options_;
        statistic_views::allocation_stack_trace_helper const helper_;
        std::map<uint64_t, process_heap_entry> const allocated_entries_;
        std::map<uint64_t, process_heap_entry> const free_entries_;
    };
}
