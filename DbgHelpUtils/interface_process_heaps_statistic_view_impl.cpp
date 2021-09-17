#include "interface_process_heaps_statistic_view_impl.h"

#include <functional>
#include <sstream>

#include "by_application_callsite_frequency.h"
#include "by_size_frequency_view.h"
#include "by_size_ranges_frequency_view.h"
#include "by_stacktrace_frequency.h"
#include "process_heap_entry.h"
#include "statistic_view_options.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::heap
{
    std::map<process_heaps_statistic_view::view_type, std::function<std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl>(process_heaps_statistic_view::view_type, statistic_views::allocation_stack_trace_helper const&, statistic_views::statistic_view_options const&, process_heaps const&, std::map<uint64_t, process_heap_entry> const&, std::map<uint64_t, process_heap_entry> const&)>> const process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::Factory
    {
        {view_type::by_size_frequency, make_by_size_frequency},
        {view_type::by_size_ranges_frequency, make_by_size_ranges_frequency},
        {view_type::by_stacktrace_frequency, make_by_stacktrace_frequency},
        {view_type::by_application_callsite_frequency, make_by_application_callsite_frequency},
    };

    process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::interface_process_heaps_statistic_view_impl(view_type const view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    : view_{view}
    , helper_{helper}
    , process_{process}
    , allocated_entries_{allocated_entries}
    , free_entries_{free_entries}
    {
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::make(view_type view, statistic_views::allocation_stack_trace_helper const& helper, statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        auto const it = Factory.find(view);
        if(it == Factory.end())
        {
            throw exceptions::wide_runtime_error{(std::wostringstream{} << "View (" << static_cast<int>(view) << ") not supported").str()};
        }

        return it->second(view, helper, statistic_view_options, process, allocated_entries, free_entries);
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::make_by_size_frequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, [[maybe_unused]] statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        return std::make_unique<statistic_views::by_size_frequency_view>(view, helper, process, allocated_entries, free_entries);
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::make_by_size_ranges_frequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        return std::make_unique<statistic_views::by_size_ranges_frequency_view>(view, statistic_view_options.range_size(), helper, process, allocated_entries, free_entries);
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::make_by_stacktrace_frequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, [[maybe_unused]] statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        return std::make_unique<statistic_views::by_stacktrace_frequency>(view, helper, process, allocated_entries, free_entries);
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::make_by_application_callsite_frequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, [[maybe_unused]] statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        return std::make_unique<statistic_views::by_application_callsite_frequency>(view, helper, process, allocated_entries, free_entries);
    }
}
