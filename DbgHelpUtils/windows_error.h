﻿#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

namespace dlg_help_utils::windows_error
{
    std::wstring get_windows_error_string(DWORD ec, std::wstring const& dll_source_path);

    inline std::wstring get_windows_error_string(DWORD const ec)
    {
        return get_windows_error_string(ec, {});
    }
}
