#pragma once

class dump_file_options;

namespace dlg_help_utils
{
    namespace dbg_help
    {
        class symbol_engine;
    }

    class mini_dump;
}

void dump_mini_dump_exception_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                          dump_file_options const& options,
                                          dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
