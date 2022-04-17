#pragma once
#include <cstdint>
#include <optional>
#include <vector>

namespace dlg_help_utils::dbg_help
{
    struct symbol_address_info;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::heap
{
    class process_heap_entry;
}

namespace dlg_help_utils::heap::statistic_views
{
    class system_module_list;

    class allocation_stack_trace_helper
    {
    public:
        allocation_stack_trace_helper(stream_stack_dump::mini_dump_memory_walker const& walker, system_module_list const& system_module_list);

        [[nodiscard]] std::optional<dbg_help::symbol_address_info> find_common_allocation_callsite(process_heap_entry const& entry) const;
        [[nodiscard]] std::optional<dbg_help::symbol_address_info> find_common_allocation_callsite(std::vector<process_heap_entry> const& entries) const;
        [[nodiscard]] std::vector<uint64_t> find_common_allocation_stack_trace(std::optional<dbg_help::symbol_address_info> const& common_allocation_callsite, std::vector<process_heap_entry> const& entries) const;

    private:
        stream_stack_dump::mini_dump_memory_walker const& walker_;
        system_module_list const& system_module_list_;
    };
}
