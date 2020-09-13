#pragma once
#include <string_view>

namespace dlg_help_utils::exception_utils
{
    std::wstring_view exception_code_to_string(uint32_t exception_code);
    std::wstring exception_flags_to_string(uint32_t exception_flags);
}
