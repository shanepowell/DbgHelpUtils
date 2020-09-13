#pragma once

class dump_file_options;

namespace dlg_help_utils
{
    class mini_dump;
}

void dump_mini_dump_memory_list_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                            dump_file_options const& options);
void dump_mini_dump_memory64_list_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index,
                                              dump_file_options const& options);
void dump_mini_dump_memory_info_list_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
void dump_mini_dump_system_memory_info_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
