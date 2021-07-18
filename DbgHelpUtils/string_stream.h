#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <string_view>

namespace dlg_help_utils
{
    class mini_dump;

    namespace string_stream
    {
        std::wstring_view to_string(mini_dump const& dump, RVA rva);
        std::wstring_view to_string(mini_dump const& dump, RVA64 rva);
    }
}
