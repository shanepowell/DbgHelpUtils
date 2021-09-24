#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"

#include <format>
#include <ostream>

namespace dlg_help_utils::system_time_utils
{
    inline std::wstring to_wstring(SYSTEMTIME const& st)
    {
        return std::format(L"{0}/{1}/{2} {3:>02}:{4:>02}:{5:>02}.{6}", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    }

    inline std::wostream& operator<<(std::wostream& os, SYSTEMTIME const& st)
    {
        os << to_wstring(st);
        return os;
    }
}
