﻿#include "by_stacktrace_frequency_view.h"

#include <numeric>

#include "allocation_stack_trace_helper.h"
#include "process_heaps_statistic_bucket_view.h"
#include "process_heap_entry.h"

namespace dlg_help_utils::heap::statistic_views
{
    by_stacktrace_frequency_view::by_stacktrace_frequency_view(view_type const view, allocation_stack_trace_helper const& helper, statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries)
    : interface_process_heaps_statistic_view_impl(view, helper, statistic_view_options, process, allocated_entries, free_entries)
    {
    }

    std::experimental::generator<process_heaps_statistic_bucket_view> by_stacktrace_frequency_view::buckets() const
    {
        std::map<uint64_t, bucket_entries> buckets;
        for(const auto& entry : allocated_entries() | std::views::values)
        {
            auto& bucket = get_bucket(buckets, entry);
            bucket.entries.emplace_back(entry);
        }

        for(const auto& entry : free_entries() | std::views::values)
        {
            auto& bucket = get_bucket(buckets, entry);
            bucket.free_entries.emplace_back(entry);
        }

        for(auto bucket : sorted_range(buckets | std::views::transform([this, &buckets](std::pair<uint64_t, bucket_entries> const& data)
            {
                auto const& [key, bucket] = data;
                const auto common_allocation_callsite = helper().find_common_allocation_callsite(bucket.entries);
                return process_heaps_statistic_bucket_view{bucket.start_range
                    , bucket.end_range
                    , calculate_bucket_range_count_percent(bucket, allocated_entries().size())
                    , calculate_bucket_range_size_percent(buckets, key)
                    , common_allocation_callsite
                    , helper().find_common_allocation_stack_trace(common_allocation_callsite, bucket.entries)
                    , bucket.entries
                    , bucket.free_entries};
            }), buckets.size()))
        {
            co_yield bucket;
        }
    }

    by_stacktrace_frequency_view::bucket_entries& by_stacktrace_frequency_view::get_bucket(std::map<uint64_t, bucket_entries>& buckets, process_heap_entry const& entry)
    {
        auto const key = entry.ust_address();
        bucket_entries* entries;
        if(const auto it = buckets.find(key);
            it != buckets.end())
        {
            entries = &it->second;
            if(entries->start_range > entry.user_requested_size())
            {
                entries->start_range = entry.user_requested_size();
            }
            if(entries->end_range < entry.user_requested_size())
            {
                entries->end_range = entry.user_requested_size();
            }
        }
        else
        {
            entries = &buckets.insert(std::make_pair(key, bucket_entries{})).first->second;
            entries->start_range = entry.user_requested_size();
            entries->end_range = entry.user_requested_size();
        }
        return *entries;
    }

    double by_stacktrace_frequency_view::calculate_bucket_range_count_percent(bucket_entries const& bucket, size_t const max_entries)
    {
        return static_cast<double>(bucket.entries.size()) / static_cast<double>(max_entries) * 100.0;
    }

    double by_stacktrace_frequency_view::calculate_bucket_range_size_percent(std::map<uint64_t, bucket_entries> const& buckets, uint64_t const key)
    {
        uint64_t bucket_size{0};
        uint64_t total_sum{0};
        for(auto const& [bucket_key, bucket] : buckets)
        {
            auto const bucket_sum = sum_entries(bucket.entries);
            if(bucket_key == key)
            {
                bucket_size = bucket_sum;
            }
            total_sum += bucket_sum;
        }

        return static_cast<double>(bucket_size) / static_cast<double>(total_sum) * 100.0;
    }

    uint64_t by_stacktrace_frequency_view::sum_entries(std::vector<process_heap_entry> const& entries)
    {
        return std::accumulate(entries.begin(), entries.end(), static_cast<uint64_t>(0), [](uint64_t const value, process_heap_entry const& entry) { return value + entry.user_requested_size().count(); });
    }
}
