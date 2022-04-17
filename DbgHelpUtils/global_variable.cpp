#include "global_variable.h"

#include "mini_dump_memory_walker.h"

namespace dlg_help_utils::process
{
    global_variable::global_variable(stream_stack_dump::mini_dump_memory_walker const& walker, dbg_help::symbol_type_info const& symbol_info)
    : walker_{walker}
    , symbol_info_{symbol_info}
    {
    }

    mini_dump_memory_stream global_variable::stream() const
    {
        auto const address = symbol_type().address();
        if(auto const length = symbol_type().length(); address.has_value() && length.has_value())
        {
            return walker_.get_process_memory_stream(address.value(), length.value());
        }

        return {};
    }
}
