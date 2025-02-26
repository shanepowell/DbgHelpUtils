#include "process_heaps_statistics.h"

#include "process_heaps.h"
#include "process_heaps_statistic_view.h"
#include "process_heap_entry.h"

namespace dlg_help_utils::heap
{
    process_heaps_statistics::process_heaps_statistics(process_heaps const& process, system_module_list const& system_module_list, statistic_views::statistic_view_options const& statistic_view_options)
    : process_{process}
    , statistic_view_options_{statistic_view_options}
    , helper_{process.peb().walker(), system_module_list}
    , allocated_entries_{get_allocated_entries()}
    , free_entries_{get_free_entries()}
    {
    }

    process_heaps_statistic_view process_heaps_statistics::view_by_size_frequency() const
    {
        return process_heaps_statistic_view{process_heaps_statistic_view::view_type::by_size_frequency, helper_, statistic_view_options_, process_, allocated_entries_, free_entries_};
    }

    process_heaps_statistic_view process_heaps_statistics::view_by_size_ranges_frequency() const
    {
        return process_heaps_statistic_view{process_heaps_statistic_view::view_type::by_size_ranges_frequency, helper_, statistic_view_options_, process_, allocated_entries_, free_entries_};
    }

    process_heaps_statistic_view process_heaps_statistics::view_by_stacktrace_frequency() const
    {
        return process_heaps_statistic_view{process_heaps_statistic_view::view_type::by_stacktrace_frequency, helper_, statistic_view_options_, process_, allocated_entries_, free_entries_};
    }

    process_heaps_statistic_view process_heaps_statistics::view_by_application_callsite_frequency() const
    {
        return process_heaps_statistic_view{process_heaps_statistic_view::view_type::by_application_callsite_frequency, helper_, statistic_view_options_, process_, allocated_entries_, free_entries_};
    }

    std::map<uint64_t, process_heap_entry> process_heaps_statistics::get_allocated_entries() const
    {
        return get_all_entries(process_.entries());
    }

    std::map<uint64_t, process_heap_entry> process_heaps_statistics::get_free_entries() const
    {
        return get_all_entries(process_.free_entries());
    }

    // ReSharper disable once CppParameterMayBeConst
    // ReSharper disable once CppPassValueParameterByConstReference
    std::map<uint64_t, process_heap_entry> process_heaps_statistics::get_all_entries(std::generator<process_heap_entry> entries)
    {
        std::map<uint64_t, process_heap_entry> rv;
        for (auto const& entry : entries)
        {
            rv.insert(std::make_pair(entry.user_address(), entry));
        }

        return rv;
    }
}
