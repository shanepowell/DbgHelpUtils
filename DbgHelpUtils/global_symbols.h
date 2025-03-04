#pragma once
#include <string>
#include <vector>
#include "generator.h"

#include "sym_tag_enum.h"

namespace dlg_help_utils::dbg_help
{
    class symbol_type_info;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::process
{
    class global_symbol;

    class global_symbols
    {
    public:
        global_symbols(stream_stack_dump::mini_dump_memory_walker const& walker, std::wstring const& match_pattern = {});

        [[nodiscard]] generator<global_symbol> all_symbols() const;

    private:
        static bool is_variable_symbol(dbg_help::sym_tag_enum tag);

    private:
        stream_stack_dump::mini_dump_memory_walker const* walker_;
        std::vector<dbg_help::symbol_type_info> symbols_;
    };
}
