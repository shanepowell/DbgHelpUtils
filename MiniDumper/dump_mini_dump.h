#pragma once
#include <string>

class dump_file_options;

namespace dlg_help_utils
{
    class mini_dump;
}

bool process_dump_file(std::wstring const& file_name, dump_file_options const& options);
void dump_mini_dump_header(dlg_help_utils::mini_dump const& dump_file, dump_file_options const& options);
void process_invalid_user_mode_dump(dlg_help_utils::mini_dump const& dump_file, dump_file_options const& options);
void process_x86_kernel_memory_dump(dlg_help_utils::mini_dump const& dump_file, dump_file_options const& options);
void process_x64_kernel_memory_dump(dlg_help_utils::mini_dump const& dump_file, dump_file_options const& options);
void process_user_mode_dump(dlg_help_utils::mini_dump const& dump_file, dump_file_options const& options);
void display_version_information();
