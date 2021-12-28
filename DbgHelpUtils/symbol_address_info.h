#pragma once
#include <string>
#include <vector>

#include "local_variable.h"

namespace dlg_help_utils::dbg_help
{
    class symbol_type_info;
    struct local_variable;

    struct symbol_address_info
    {
        std::wstring module_name;
        std::wstring symbol_name;
        std::wstring file_name;
        DWORD line_number{};
        DWORD64 module_displacement{};
        DWORD64 symbol_displacement{};
        DWORD line_displacement{};
        bool in_line{false};
        bool found{false};
        bool frame_content_found{false};
        DWORD64 address;
        DWORD64 stack;
        std::vector<local_variable> local_variables;
        std::vector<local_variable> parameters;
    };
}
