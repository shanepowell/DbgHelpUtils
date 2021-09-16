#pragma once
#include <string_view>

namespace dlg_help_utils::string_compare
{
    [[nodiscard]] inline bool iequals(std::wstring_view const& str1, std::wstring_view const& str2)
    {
        if(str1.length() != str2.length())
        {
            return false;
        }

        return _wcsnicmp(str1.data(), str2.data(), str1.length()) == 0;
    }
}
    