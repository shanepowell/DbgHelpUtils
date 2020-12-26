#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <string>

namespace dlg_help_utils::guid_utils
{
    std::wstring to_string(GUID const& guid);
}
