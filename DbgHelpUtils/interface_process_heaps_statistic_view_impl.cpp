#include "interface_process_heaps_statistic_view_impl.h"

#include <functional>
#include <sstream>

#include "by_size_frequency_view.h"
#include "process_heap_entry.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::heap
{
    std::map<process_heaps_statistic_view::view_type, std::function<std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl>(process_heaps_statistic_view::view_type, statistic_views::allocation_stack_trace_helper const&, process_heaps const&, std::map<uint64_t, process_heap_entry> const&, std::map<uint64_t, process_heap_entry> const&)>> const process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::Factory
    {
        {view_type::by_size_frequency, MakeBySizeFrequency},
        {view_type::by_size_ranges_frequency, MakeBySizeRangesFrequency},
        {view_type::by_stacktrace_frequency, MakeByStacktraceFrequency},
        {view_type::by_application_callsite_frequency, MakeByApplicationCallsiteFrequency},
    };

    process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::interface_process_heaps_statistic_view_impl(view_type const view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    : view_{view}
    , helper_{helper}
    , process_{process}
    , allocated_entries_{allocated_entries}
    , free_entries_{free_entries}
    {
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::make(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        auto const it = Factory.find(view);
        if(it == Factory.end())
        {
            throw exceptions::wide_runtime_error{(std::wostringstream{} << "View (" << static_cast<int>(view) << ") not supported").str()};
        }

        return it->second(view, helper, process, allocated_entries, free_entries);
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::MakeBySizeFrequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        return std::make_unique<statistic_views::by_size_frequency_view>(view, helper, process, allocated_entries, free_entries);
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::MakeBySizeRangesFrequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        return std::make_unique<statistic_views::by_size_frequency_view>(view, helper, process, allocated_entries, free_entries);
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::MakeByStacktraceFrequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        return std::make_unique<statistic_views::by_size_frequency_view>(view, helper, process, allocated_entries, free_entries);
    }

    std::unique_ptr<process_heaps_statistic_view::interface_process_heaps_statistic_view_impl> process_heaps_statistic_view::interface_process_heaps_statistic_view_impl::MakeByApplicationCallsiteFrequency(view_type view, statistic_views::allocation_stack_trace_helper const& helper, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    {
        return std::make_unique<statistic_views::by_size_frequency_view>(view, helper, process, allocated_entries, free_entries);
    }
}
