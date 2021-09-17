#include "process_heaps_statistic_view.h"

#include "interface_process_heaps_statistic_view_impl.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    process_heaps_statistic_view::process_heaps_statistic_view(view_type const view, statistic_views::allocation_stack_trace_helper const& helper, statistic_views::statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    : impl_{interface_process_heaps_statistic_view_impl::make(view, helper, statistic_view_options, process, allocated_entries, free_entries)}
    {
    }

    process_heaps_statistic_view::~process_heaps_statistic_view() = default;

    process_heaps_statistic_view::view_type process_heaps_statistic_view::view() const
    {
        return impl_->view();
    }

    bool process_heaps_statistic_view::is_range_single_value() const
    {
        return impl_->is_range_single_value();
    }

    std::experimental::generator<process_heaps_statistic_bucket_view> process_heaps_statistic_view::buckets() const
    {
        return impl_->buckets();
    }

    size_units::base_16::bytes process_heaps_statistic_view::allocated_average() const
    {
        return impl_->allocated_average();
    }

    size_units::base_16::bytes process_heaps_statistic_view::allocated_total() const
    {
        return impl_->allocated_total();
    }

    size_units::base_16::bytes process_heaps_statistic_view::free_total() const
    {
        return impl_->free_total();
    }

    size_units::base_16::bytes process_heaps_statistic_view::overhead_total() const
    {
        return impl_->overhead_total();
    }

    size_t process_heaps_statistic_view::allocated_count() const
    {
        return impl_->allocated_count();
    }

    size_t process_heaps_statistic_view::free_count() const
    {
        return impl_->free_count();
    }

    std::wstring process_heaps_statistic_view::to_wstring(view_type const type)
    {
        switch(type)
        {
        case view_type::by_size_frequency: return L"by size frequency"s;
        case view_type::by_size_ranges_frequency: return L"by size ranges frequency"s;
        case view_type::by_stacktrace_frequency: return L"by stracktrace frequency"s;
        case view_type::by_application_callsite_frequency: return L"by application callsite frequency"s;
        }

        throw exceptions::wide_runtime_error{L"unknown view_type value"s};
    }
}
