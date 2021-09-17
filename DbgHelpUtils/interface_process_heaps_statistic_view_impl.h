#pragma once
#include "process_heaps.h"
#include "process_heaps_statistic_view.h"
#include "size_units.h"

namespace dlg_help_utils::heap
{
    namespace statistic_views
    {
        class allocation_stack_trace_helper;
    }

    class process_heaps_statistic_view::interface_process_heaps_statistic_view_impl  // NOLINT(cppcoreguidelines-special-member-functions)
    {
    public:
        interface_process_heaps_statistic_view_impl(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);
        virtual ~interface_process_heaps_statistic_view_impl() = default;

        static std::unique_ptr<interface_process_heaps_statistic_view_impl> make(view_type view, statistic_views::allocation_stack_trace_helper const& helper, statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);

        [[nodiscard]] virtual std::experimental::generator<process_heaps_statistic_bucket_view> buckets() const = 0;

        [[nodiscard]] size_units::base_16::bytes allocated_average() const;
        [[nodiscard]] size_units::base_16::bytes allocated_total() const;
        [[nodiscard]] size_units::base_16::bytes free_total() const;
        [[nodiscard]] size_units::base_16::bytes overhead_total() const;
        [[nodiscard]] size_t allocated_count() const { return allocated_entries_.size(); }
        [[nodiscard]] size_t free_count() const { return free_entries_.size(); }

        [[nodiscard]] view_type view() const { return view_; }
        [[nodiscard]] virtual bool is_range_single_value() const = 0;
        [[nodiscard]] process_heaps const& process() const { return process_; }
        [[nodiscard]] std::map<uint64_t, process_heap_entry> const& allocated_entries() const { return allocated_entries_; }
        [[nodiscard]] std::map<uint64_t, process_heap_entry> const& free_entries() const { return free_entries_; }

    protected:
        [[nodiscard]] statistic_views::allocation_stack_trace_helper const& helper() const { return helper_; }

    private:
        [[nodiscard]] static uint64_t sum_entries(std::map<uint64_t, process_heap_entry> const& entries);

        template<typename Op>
        [[nodiscard]] static uint64_t accumulate_entries(std::map<uint64_t, process_heap_entry> const& entries, Op op);

        [[nodiscard]] static std::unique_ptr<interface_process_heaps_statistic_view_impl> make_by_size_frequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);
        [[nodiscard]] static std::unique_ptr<interface_process_heaps_statistic_view_impl> make_by_size_ranges_frequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);
        [[nodiscard]] static std::unique_ptr<interface_process_heaps_statistic_view_impl> make_by_stacktrace_frequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);
        [[nodiscard]] static std::unique_ptr<interface_process_heaps_statistic_view_impl> make_by_application_callsite_frequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);

        static std::map<view_type, std::function<std::unique_ptr<interface_process_heaps_statistic_view_impl>(view_type, statistic_views::allocation_stack_trace_helper const&, statistic_views::statistic_view_options const&, process_heaps const&, std::map<uint64_t, process_heap_entry> const&, std::map<uint64_t, process_heap_entry> const&)>> const Factory;

    private:
        view_type const view_;
        statistic_views::allocation_stack_trace_helper const& helper_;
        process_heaps const& process_;
        std::map<uint64_t, process_heap_entry> const& allocated_entries_;
        std::map<uint64_t, process_heap_entry> const& free_entries_;
    };
}
