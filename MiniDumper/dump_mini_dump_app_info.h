#pragma once

#include <ostream>

class dump_file_options;

namespace dlg_help_utils
{
    namespace dbg_help
    {
        class symbol_engine;
    }

    class mini_dump;
}

void dump_mini_dump_token_stream_data(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                      dump_file_options const& options);
void dump_mini_dump_function_table_stream_data(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                               dump_file_options const& options);
void dump_mini_dump_handle_operation_list_stream_data(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                                      dump_file_options const& options,
                                                      dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_handle_data_stream_data(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                            dump_file_options const& options);
void dump_mini_dump_ipt_trace_stream_data(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, size_t index);
