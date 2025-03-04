#include "global_symbols.h"

#include "global_symbol.h"
#include "mini_dump_memory_walker.h"
#include "symbol_engine.h"

namespace dlg_help_utils::process
{
    global_symbols::global_symbols(stream_stack_dump::mini_dump_memory_walker const& walker, std::wstring const& match_pattern)
    : walker_{&walker}
    , symbols_{walker.symbol_walk(match_pattern)}
    {
    }

    generator<global_symbol> global_symbols::all_symbols() const
    {
        for (auto const& symbol : symbols_)
        {
            if(auto tag = symbol.sym_tag(); is_variable_symbol(tag.value_or(dbg_help::sym_tag_enum::Null)))
            {
                co_yield global_symbol{*walker_, symbol};
            }
        }
    }

    bool global_symbols::is_variable_symbol(dbg_help::sym_tag_enum const tag)
    {
        return tag == dbg_help::sym_tag_enum::PublicSymbol || tag == dbg_help::sym_tag_enum::Data;
    }
}
