#pragma once
#include "mini_dump_memory_stream.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::process
{
    class global_variable
    {
    public:
        global_variable(stream_stack_dump::mini_dump_memory_walker const& walker, dbg_help::symbol_type_info const& symbol_info);

        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return symbol_info_; }
        [[nodiscard]] mini_dump_memory_stream stream() const;

    private:
        stream_stack_dump::mini_dump_memory_walker const& walker_;
        dbg_help::symbol_type_info symbol_info_;
    };
}
