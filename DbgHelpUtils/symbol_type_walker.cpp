#include "symbol_type_walker.h"

#include "mini_dump_memory_walker.h"

namespace dlg_help_utils::stream_stack_dump
{
    symbol_type_walker::symbol_type_walker(mini_dump_memory_walker const& walker, std::wstring const& match_pattern)
    : walker_{walker}
    , symbols_{walker.symbol_walk(match_pattern)}
    {
    }
}
