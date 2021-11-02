#pragma once
#include <string>
#include <vector>
#include <experimental/generator>

#include "sym_tag_enum.h"

namespace dlg_help_utils::dbg_help
{
    class symbol_type_info;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::process
{
    class global_variable;

    class global_variables
    {
    public:
        global_variables(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& match_pattern = {});

        [[nodiscard]] std::experimental::generator<global_variable> all_variables() const;

    private:
        static bool is_variable_symbol(dbg_help::sym_tag_enum tag);

    private:
        stream_stack_dump::mini_dump_stack_walk const& walker_;
        std::vector<dbg_help::symbol_type_info> symbols_;
    };
}
