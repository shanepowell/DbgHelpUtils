#pragma once
#include "windows_setup.h"
#include <string>

namespace dlg_help_utils::windows_error
{
    std::wstring get_windows_error_string(DWORD ec, std::wstring const& dll_source_path);

    inline std::wstring get_windows_error_string(DWORD const ec)
    {
        return get_windows_error_string(ec, {});
    }
}
