#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <string>

namespace dlg_help_utils::windows_error
{
    [[noreturn]] void throw_windows_api_error(std::wstring_view const& api, std::wstring_view const& optional_data, DWORD ec = GetLastError());
    [[noreturn]] void throw_windows_api_error(std::wstring_view const& api, DWORD ec = GetLastError());

    std::wstring get_windows_error_string(DWORD ec, std::wstring const& dll_source_path);

    inline std::wstring get_windows_error_string(DWORD const ec)
    {
        return get_windows_error_string(ec, {});
    }
}
