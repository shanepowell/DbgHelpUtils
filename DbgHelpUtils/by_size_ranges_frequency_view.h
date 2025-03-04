#pragma once
#include <cstdint>
#include <vector>

#include "interface_process_heaps_statistic_view_impl.h"
#include "process_heaps_statistic_view.h"

namespace dlg_help_utils::heap::statistic_views
{
    class by_size_ranges_frequency_view
    : public process_heaps_statistic_view::interface_process_heaps_statistic_view_impl
    {
    public:
        using view_type = process_heaps_statistic_view::view_type;

        by_size_ranges_frequency_view(view_type view, allocation_stack_trace_helper const& helper, statistic_view_options const& statistic_view_options, process_heaps const& process, std::map<uint64_t, process_heap_entry> const& allocated_entries, std::map<uint64_t, process_heap_entry> const& free_entries);

        [[nodiscard]] bool is_range_single_value() const override { return false; }
        [[nodiscard]] generator<process_heaps_statistic_bucket_view> buckets() const override;

    private:
        struct bucket_entries
        {
            std::vector<process_heap_entry> entries;
            std::vector<process_heap_entry> free_entries;
        };

        [[nodiscard]] bucket_entries& get_bucket(std::map<uint64_t, bucket_entries>& buckets, process_heap_entry const& entry) const;
        [[nodiscard]] static double calculate_bucket_range_count_percent(bucket_entries const& bucket, size_t max_entries);
        [[nodiscard]] static double calculate_bucket_range_size_percent(std::map<uint64_t, bucket_entries> const& buckets, uint64_t key);
        [[nodiscard]] static uint64_t sum_entries(std::vector<process_heap_entry> const& entries);

    private:
        size_units::base_16::bytes range_size_;
    };
}
