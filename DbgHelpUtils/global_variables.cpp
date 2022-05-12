#include "global_variables.h"

#include "global_variable.h"
#include "mini_dump_memory_walker.h"
#include "symbol_engine.h"

namespace dlg_help_utils::process
{
    global_variables::global_variables(stream_stack_dump::mini_dump_memory_walker const& walker, std::wstring const& match_pattern)
    : walker_{&walker}
    , symbols_{walker.symbol_walk(match_pattern)}
    {
    }

    std::experimental::generator<global_variable> global_variables::all_variables() const
    {
        for (auto const& symbol : symbols_)
        {
            if(auto tag = symbol.sym_tag(); is_variable_symbol(tag.value_or(dbg_help::sym_tag_enum::Null)))
            {
                co_yield global_variable{*walker_, symbol};
            }
        }
    }

    bool global_variables::is_variable_symbol(dbg_help::sym_tag_enum const tag)
    {
        return tag == dbg_help::sym_tag_enum::PublicSymbol || tag == dbg_help::sym_tag_enum::Data;
    }
}
