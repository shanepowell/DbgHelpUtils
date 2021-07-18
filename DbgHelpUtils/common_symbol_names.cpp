#include "common_symbol_names.h"

using namespace std::string_literals;

namespace dlg_help_utils::common_symbol_names
{
    std::wstring const peb_structure_symbol_name = L"ntdll!_PEB"s;
    std::wstring const peb_ldr_structure_symbol_name = L"ntdll!_PEB_LDR_DATA"s;
    std::wstring const rtl_user_process_parameters_structure_symbol_name = L"ntdll!_RTL_USER_PROCESS_PARAMETERS"s;
    std::wstring const teb_structure_symbol_name = L"ntdll!_TEB"s;

    std::wstring const peb_structure_ldr_field_symbol_name = L"ldr"s;
    std::wstring const peb_structure_process_parameters_field_symbol_name = L"ProcessParameters"s;
    std::wstring const rtl_user_process_parameters_structure_environment_field_symbol_name = L"Environment"s;
    std::wstring const teb_structure_process_environment_block_field_symbol_name = L"ProcessEnvironmentBlock"s;
}
