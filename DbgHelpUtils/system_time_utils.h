#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"

#include <format>
#include <ostream>

namespace dlg_help_utils::system_time_utils
{
    inline std::wstring to_raw_wstring(SYSTEMTIME const& st)
    {
        return std::format(L"{0:0>4}/{1:0>2}/{2:0>2} {3:0>2}:{4:0>2}:{5:0>2}.{6}", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    }
}
