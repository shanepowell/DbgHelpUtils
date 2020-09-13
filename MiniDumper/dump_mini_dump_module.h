#pragma once

class dump_file_options;

namespace dlg_help_utils
{
    class mini_dump;
}

void dump_mini_dump_module_list_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                            dump_file_options const& options);
void dump_mini_dump_unloaded_module_list_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
