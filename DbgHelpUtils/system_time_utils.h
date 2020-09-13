#pragma once
#define WIN32_LEAN_AND_MEAN
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <ostream>
#include <iomanip>

namespace dlg_help_utils::system_time_utils
{
    inline std::wostream& operator<<(std::wostream& os, SYSTEMTIME const& st)
    {
        os << st.wYear << L'/' << st.wMonth << L'/' << st.wDay << L' ' << std::setw(2) << std::setfill(L'0') << st.wHour
            << L':' << std::setw(2) << st.wMinute << L':' << st.wSecond << L'.' << st.wMilliseconds;
        return os;
    }
}
