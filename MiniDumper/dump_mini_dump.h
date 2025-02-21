﻿#pragma once
#include <memory>
#include <string>

class dump_file_options;

namespace dlg_help_utils
{
    class cache_manager;
    class mini_dump;
}

bool process_dump_file(std::wostream& log, std::wstring const& file_name, std::wstring const& base_diff_file_name, dump_file_options & options);
void dump_mini_dump_header(std::wostream& log, dlg_help_utils::mini_dump const& dump_file, dump_file_options const& options);
void process_invalid_user_mode_dump(std::wostream& log, dlg_help_utils::mini_dump const& dump_file, dump_file_options const& options);
void process_x86_kernel_memory_dump(std::wostream& log, dlg_help_utils::mini_dump const& dump_file, dump_file_options const& options);
void process_x64_kernel_memory_dump(std::wostream& log, dlg_help_utils::mini_dump const& dump_file, dump_file_options const& options);
void process_user_mode_dump(std::wostream& log, dlg_help_utils::mini_dump const& dump_file, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options & options);
void display_version_information(std::wostream& log);
