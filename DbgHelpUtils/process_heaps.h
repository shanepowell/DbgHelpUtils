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
    class process_heap_entry;

    class process_heaps
    {
    public:
        process_heaps(mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine);

        process::process_environment_block const& peb() const { return peb_; }

        [[nodiscard]] std::experimental::generator<process_heap_entry> entries() const;

    private:
        process::process_environment_block const peb_;
    };
}
