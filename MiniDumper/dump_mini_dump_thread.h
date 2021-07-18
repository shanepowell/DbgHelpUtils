#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"

class dump_file_options;

namespace dlg_help_utils
{
    namespace dbg_help
    {
        class symbol_engine;
    }

    class mini_dump;
    class stream_thread_context;
}

void dump_mini_dump_thread_context(dlg_help_utils::stream_thread_context const& thread_context,
                                   dump_file_options const& options);
void dump_mini_dump_x64_thread_context(CONTEXT const& context);
void dump_mini_dump_x86_thread_context(WOW64_CONTEXT const& context, bool has_extended_registers);
void dump_mini_dump_thread_names_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
void dump_mini_dump_thread_list_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                            dump_file_options const& options,
                                            dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_thread_list_ex_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                               dump_file_options const& options,
                                               dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_thread_info_list_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
void load_and_dump_teb(dlg_help_utils::mini_dump const& mini_dump,
    dlg_help_utils::dbg_help::symbol_engine& symbol_engine,
    ULONG64 teb_address);
