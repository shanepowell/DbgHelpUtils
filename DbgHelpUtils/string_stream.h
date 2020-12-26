#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
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
