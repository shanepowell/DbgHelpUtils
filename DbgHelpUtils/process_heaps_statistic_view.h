#pragma once
#include <map>
#include <string>
#include "generator.h"

#include "size_units.h"

namespace dlg_help_utils::heap
{
    namespace statistic_views
    {
        class by_application_callsite_frequency_view;
        class by_stacktrace_frequency_view;
        class statistic_view_options;
        class allocation_stack_trace_helper;
        class by_size_frequency_view;
        class by_size_ranges_frequency_view;
    }

    class process_heaps;
    class process_heap_entry;
    class process_heaps_statistic_bucket_view;

    class process_heaps_statistic_view
    {
    public:
        enum class view_type : uint8_t
        {
            by_size_frequency,
            by_size_ranges_frequency,
            by_stacktrace_frequency,
            by_application_callsite_frequency
        };

        enum class sort_column_type : uint8_t
        {
            size,
            allocated_average,
            allocated_total,
            allocated_count,
            free_total,
            free_count,
            overhead_total,
            range_count_percent,
            range_size_percent
        };

        enum class sort_order_type : uint8_t
        {
            ascending,
            descending
        };

        process_heaps_statistic_view(view_type view, statistic_views::allocation_stack_trace_helper const& helper, statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);
        ~process_heaps_statistic_view();

        process_heaps_statistic_view(process_heaps_statistic_view const&) = delete;
        process_heaps_statistic_view(process_heaps_statistic_view &&) = default;

        process_heaps_statistic_view& operator=(process_heaps_statistic_view const&) = delete;
        process_heaps_statistic_view& operator=(process_heaps_statistic_view &&) = default;

        [[nodiscard]] view_type view() const;
        [[nodiscard]] bool is_range_single_value() const;

        [[nodiscard]] sort_column_type sort_column() const;
        void sort_column(sort_column_type value);
        [[nodiscard]] sort_order_type sort_order() const;
        void sort_order(sort_order_type value);

        [[nodiscard]] generator<process_heaps_statistic_bucket_view> buckets() const;

        [[nodiscard]] size_units::base_16::bytes allocated_average() const;
        [[nodiscard]] size_units::base_16::bytes allocated_total() const;
        [[nodiscard]] size_units::base_16::bytes free_total() const;
        [[nodiscard]] size_units::base_16::bytes overhead_total() const;
        [[nodiscard]] size_t allocated_count() const;
        [[nodiscard]] size_t free_count() const;

        static std::wstring to_wstring(view_type type);
    private:
        class interface_process_heaps_statistic_view_impl;
        friend class statistic_views::by_size_frequency_view;
        friend class statistic_views::by_size_ranges_frequency_view;
        friend class statistic_views::by_stacktrace_frequency_view;
        friend class statistic_views::by_application_callsite_frequency_view;
        std::unique_ptr<interface_process_heaps_statistic_view_impl> impl_;
    };

}
