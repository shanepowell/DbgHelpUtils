﻿#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"
#include <DbgHelp.h>

class dump_file_options;

namespace dlg_help_utils
{
    namespace dbg_help
    {
        class symbol_engine;
    }

    class mini_dump;
}

void dump_mini_dump_streams(dlg_help_utils::mini_dump const& dump_file);
void dump_mini_dump_stream_index(dlg_help_utils::mini_dump const& dump_file, size_t index,
                                 dump_file_options const& options,
                                 dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_stream_type(dlg_help_utils::mini_dump const& dump_file, MINIDUMP_STREAM_TYPE type,
                                dump_file_options const& options,
                                dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_all_stream_indexes(dlg_help_utils::mini_dump const& dump_file,
                                 dump_file_options const& options,
                                 dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                MINIDUMP_DIRECTORY const& entry, dump_file_options const& options,
                                dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
