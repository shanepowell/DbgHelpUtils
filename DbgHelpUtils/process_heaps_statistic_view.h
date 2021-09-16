#pragma once
#include <map>
#include <string>
#include <experimental/generator>

namespace dlg_help_utils::heap
{
    namespace statistic_views
    {
        class allocation_stack_trace_helper;
        class by_size_frequency_view;
    }

    class process_heaps;
    class process_heap_entry;
    class process_heaps_statistic_bucket_view;

    class process_heaps_statistic_view
    {
    public:
        enum class view_type
        {
            by_size_frequency,
            by_size_ranges_frequency,
            by_stacktrace_frequency,
            by_application_callsite_frequency
        };

        process_heaps_statistic_view(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);
        ~process_heaps_statistic_view();

        process_heaps_statistic_view(process_heaps_statistic_view const&) = delete;
        process_heaps_statistic_view(process_heaps_statistic_view &&) = default;

        process_heaps_statistic_view& operator=(process_heaps_statistic_view const&) = delete;
        process_heaps_statistic_view& operator=(process_heaps_statistic_view &&) = default;

        [[nodiscard]] view_type view() const;
        [[nodiscard]] bool is_range_single_value() const;

        [[nodiscard]] std::experimental::generator<process_heaps_statistic_bucket_view> buckets() const;

        static std::wstring to_wstring(view_type type);
    private:
        class interface_process_heaps_statistic_view_impl;
        friend class statistic_views::by_size_frequency_view;
        std::unique_ptr<interface_process_heaps_statistic_view_impl> impl_;
    };

}
