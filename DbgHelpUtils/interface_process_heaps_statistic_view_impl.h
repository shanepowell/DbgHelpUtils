#pragma once
#include "process_heaps.h"
#include "process_heaps_statistic_view.h"

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

        static std::unique_ptr<interface_process_heaps_statistic_view_impl> make(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);

        [[nodiscard]] virtual std::experimental::generator<process_heaps_statistic_bucket_view> buckets() const = 0;

        [[nodiscard]] view_type view() const { return view_; }
        [[nodiscard]] virtual bool is_range_single_value() const = 0;
        [[nodiscard]] process_heaps const& process() const { return process_; }
        [[nodiscard]] std::map<uint64_t, process_heap_entry> const& allocated_entries() const { return allocated_entries_; }
        [[nodiscard]] std::map<uint64_t, process_heap_entry> const& free_entries() const { return free_entries_; }

    protected:
        [[nodiscard]] statistic_views::allocation_stack_trace_helper const& helper() const { return helper_; }

    private:
        static std::unique_ptr<interface_process_heaps_statistic_view_impl> MakeBySizeFrequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);
        static std::unique_ptr<interface_process_heaps_statistic_view_impl> MakeBySizeRangesFrequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);
        static std::unique_ptr<interface_process_heaps_statistic_view_impl> MakeByStacktraceFrequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);
        static std::unique_ptr<interface_process_heaps_statistic_view_impl> MakeByApplicationCallsiteFrequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);

        static std::map<view_type, std::function<std::unique_ptr<interface_process_heaps_statistic_view_impl>(view_type, statistic_views::allocation_stack_trace_helper const&, process_heaps const&, std::map<uint64_t, process_heap_entry> const&, std::map<uint64_t, process_heap_entry> const&)>> const Factory;

    private:
        view_type const view_;
        statistic_views::allocation_stack_trace_helper const& helper_;
        process_heaps const& process_;
        std::map<uint64_t, process_heap_entry> const& allocated_entries_;
        std::map<uint64_t, process_heap_entry> const& free_entries_;
    };
}
