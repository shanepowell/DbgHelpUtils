#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <vector>

// ReSharper disable once CppUnusedIncludeDirective
#include <string_view>

namespace dlg_help_utils::mini_dump_type
{
    std::vector<std::wstring_view> to_strings(MINIDUMP_TYPE type);
}
