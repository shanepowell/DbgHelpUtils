#pragma once
#include <cstdint>

#include "mini_dump_memory_stream.h"
#include "size_units.h"

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::heap
{
    class large_alloc_entry;
    class heap_vs_entry;
    class page_range_descriptor;
    class dph_entry;
    class heap_entry;
    class heap_lfh_entry;

    class process_heap_entry
    {
    public:
        process_heap_entry(dph_entry const& entry);
        process_heap_entry(heap_entry const& entry);
        process_heap_entry(heap_lfh_entry const& entry);
        process_heap_entry(page_range_descriptor const& entry);
        process_heap_entry(heap_vs_entry const& entry);
        process_heap_entry(large_alloc_entry const& entry);

        [[nodiscard]] uint64_t user_address() const { return user_address_; }
        [[nodiscard]] size_units::base_10::bytes user_requested_size() const { return user_size_; }

        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }

        [[nodiscard]] mini_dump_memory_stream user_data() const;

        [[nodiscard]] bool contains_address(uint64_t address) const;

    private:
        process::process_environment_block const& peb_;
        uint64_t const user_address_;
        size_units::base_10::bytes const user_size_;
        std::vector<uint64_t> const allocation_stack_trace_;
        uint64_t const block_start_address_;
        uint64_t const block_end_address_;
    };
}
