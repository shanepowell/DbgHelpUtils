#pragma once
#include <vector>

#include "size_units.h"
#include "symbol_address_info.h"

namespace dlg_help_utils::heap
{
    class process_heap_entry;

    class process_heaps_statistic_bucket_view
    {
    public:
        process_heaps_statistic_bucket_view(size_units::base_16::bytes start_range
            , size_units::base_16::bytes end_range
            , double range_count_percent
            , double range_size_percent
            , std::optional<dbg_help::symbol_address_info> common_allocation_callsite
            , std::vector<uint64_t> allocation_stack_trace
            , std::vector<process_heap_entry> entries
            , std::vector<process_heap_entry> free_entries
            );

        [[nodiscard]] size_units::base_16::bytes start_range() const { return start_range_; }
        [[nodiscard]] size_units::base_16::bytes end_range() const { return end_range_; }
        [[nodiscard]] bool is_range_single_value() const { return start_range_ == end_range_; }
        [[nodiscard]] size_units::base_16::bytes allocated_average() const;
        [[nodiscard]] size_units::base_16::bytes allocated_total() const;
        [[nodiscard]] size_units::base_16::bytes free_total() const;
        [[nodiscard]] size_units::base_16::bytes overhead_total() const;
        [[nodiscard]] size_t allocated_count() const { return entries_.size(); }
        [[nodiscard]] size_t free_count() const { return free_entries_.size(); }
        [[nodiscard]] double range_count_percent() const { return range_count_percent_; }
        [[nodiscard]] double range_size_percent() const { return range_size_percent_; }
        [[nodiscard]] std::optional<dbg_help::symbol_address_info> const& common_allocation_callsite() const { return common_allocation_callsite_; }
        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }

        [[nodiscard]] std::vector<process_heap_entry> const& entries() const { return entries_; }
        [[nodiscard]] std::vector<process_heap_entry> const& free_entries() const { return free_entries_; }

    private:
        [[nodiscard]] static uint64_t sum_entries(std::vector<process_heap_entry> const& entries);

        template<typename Op>
        [[nodiscard]] static uint64_t accumulate_entries(std::vector<process_heap_entry> const& entries, Op op);

    private:
        size_units::base_16::bytes const start_range_;
        size_units::base_16::bytes const end_range_;
        double const range_count_percent_;
        double const range_size_percent_;
        std::optional<dbg_help::symbol_address_info> common_allocation_callsite_;
        std::vector<uint64_t> allocation_stack_trace_;
        std::vector<process_heap_entry> const entries_;
        std::vector<process_heap_entry> const free_entries_;
    };
}
