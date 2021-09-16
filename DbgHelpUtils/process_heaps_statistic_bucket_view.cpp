#include "process_heaps_statistic_bucket_view.h"

#include <numeric>

#include "process_heap_entry.h"

namespace dlg_help_utils::heap
{
    process_heaps_statistic_bucket_view::process_heaps_statistic_bucket_view(size_units::base_16::bytes const start_range
        , size_units::base_16::bytes const end_range
        , double const range_count_percent
        , double const range_size_percent
        , std::optional<dbg_help::symbol_address_info> common_allocation_callsite
        , std::vector<uint64_t> allocation_stack_trace
        , std::vector<process_heap_entry> entries
        , std::vector<process_heap_entry> free_entries)
    : start_range_{start_range}
    , end_range_{end_range}
    , range_count_percent_{range_count_percent}
    , range_size_percent_{range_size_percent}
    , common_allocation_callsite_{std::move(common_allocation_callsite)}
    , allocation_stack_trace_{std::move(allocation_stack_trace)}
    , entries_{std::move(entries)}
    , free_entries_{std::move(free_entries)}
    {
    }

    size_units::base_16::bytes process_heaps_statistic_bucket_view::allocated_average() const
    {
        if(entries().empty())
        {
            return size_units::base_16::bytes{0};
        }
        return size_units::base_16::bytes{allocated_total().count() / entries().size()};
    }

    size_units::base_16::bytes process_heaps_statistic_bucket_view::allocated_total() const
    {
        return size_units::base_16::bytes{sum_entries(entries())};
    }

    size_units::base_16::bytes process_heaps_statistic_bucket_view::free_total() const
    {
        return size_units::base_16::bytes{sum_entries(free_entries())};
    }

    size_units::base_16::bytes process_heaps_statistic_bucket_view::overhead_total() const
    {
        return size_units::base_16::bytes{accumulate_entries(entries(), [](uint64_t const value, process_heap_entry const& entry) { return value + entry.overhead_size().count(); })};
    }

    template <typename Op>
    uint64_t process_heaps_statistic_bucket_view::accumulate_entries(std::vector<process_heap_entry> const& entries, Op op)
    {
        return std::accumulate(entries.begin(), entries.end(), static_cast<uint64_t>(0), std::move(op));
    }

    uint64_t process_heaps_statistic_bucket_view::sum_entries(std::vector<process_heap_entry> const& entries)
    {
        return accumulate_entries(entries, [](uint64_t const value, process_heap_entry const& entry) { return value + entry.user_requested_size().count(); });
    }
}
