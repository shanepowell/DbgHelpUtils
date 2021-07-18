#pragma once
#include <string>

class dump_file_options;

namespace dlg_help_utils
{
    namespace dbg_help
    {
        class symbol_type_info;
        class symbol_engine;
    }

    class mini_dump;
}

void dump_mini_dump_symbol_type(dlg_help_utils::mini_dump const& mini_dump, std::wstring const& type_name, dump_file_options const& options,
                                dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_module_symbol_types(dlg_help_utils::mini_dump const& mini_dump, std::wstring const& module_name, dump_file_options const& options,
                                dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_address(dlg_help_utils::mini_dump const& mini_dump, std::wstring const& address, dump_file_options const& options,
                                dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_symbol_type(dlg_help_utils::dbg_help::symbol_type_info const& value, dump_file_options const& options, size_t base_offset = 0, size_t indent = 0);
void dump_mini_dump_peb(dlg_help_utils::mini_dump const& mini_dump, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
