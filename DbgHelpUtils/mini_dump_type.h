#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>
#include <vector>
#include <string_view>

namespace dlg_help_utils::mini_dump_type
{
    std::vector<std::wstring_view> to_strings(MINIDUMP_TYPE type);
}
