#pragma once
#include "process_environment_block.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace dlg_help_utils::dbg_help
{
    class symbol_engine;
}

namespace dlg_help_utils::heap
{
    class heap_subsegment;
    class heap_entry;
    class process_heap_entry;

    class process_heaps
    {
    public:
        process_heaps(mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine);

        [[nodiscard]] process::process_environment_block const& peb() const { return peb_; }

        [[nodiscard]] std::experimental::generator<process_heap_entry> entries() const;
        [[nodiscard]] std::experimental::generator<process_heap_entry> free_entries() const;

    private:
        static bool is_lfh_subsegment_in_entry(heap_entry const& entry, heap_subsegment const& subsegment);
        static bool contains_address(uint64_t start_address, uint64_t size, uint64_t address);

    private:
        process::process_environment_block const peb_;
    };
}
