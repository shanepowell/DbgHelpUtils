#pragma once
#include <string>

namespace dlg_help_utils::common_symbol_names
{
    extern std::wstring const peb_structure_symbol_name;
    extern std::wstring const peb_ldr_structure_symbol_name;
    extern std::wstring const rtl_user_process_parameters_structure_symbol_name;
    extern std::wstring const teb_structure_symbol_name;

    extern std::wstring const peb_structure_ldr_field_symbol_name;
    extern std::wstring const peb_structure_process_parameters_field_symbol_name;
    extern std::wstring const rtl_user_process_parameters_structure_environment_field_symbol_name;
    extern std::wstring const teb_structure_process_environment_block_field_symbol_name;
}
