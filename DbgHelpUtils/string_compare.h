#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include <string>

namespace dlg_help_utils::string_utils
{
    [[nodiscard]] inline bool iequals(std::wstring_view const& str1, std::wstring_view const& str2)
    {
        if(str1.length() != str2.length())
        {
            return false;
        }

        return _wcsnicmp(str1.data(), str2.data(), str1.length()) == 0;
    }

    [[nodiscard]] inline bool equals(std::wstring_view const& str1, std::wstring_view const& str2)
    {
        if(str1.length() != str2.length())
        {
            return false;
        }

        return wcsncmp(str1.data(), str2.data(), str1.length()) == 0;
    }

    [[nodiscard]] inline int icompare(std::wstring_view const& str1, std::wstring_view const& str2)
    {
        auto const max_length = std::min(str1.length(), str2.length());
        auto const result = _wcsnicmp(str1.data(), str2.data(), max_length);
        if(result < 0)
        {
            return result;
        }
        if(result > 0)
        {
            return result;
        }
        if(str1.length() > str2.length())
        {
            return 1;
        }
        if(str1.length() < str2.length())
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] inline int compare(std::wstring_view const& str1, std::wstring_view const& str2)
    {
        auto const max_length = std::min(str1.length(), str2.length());
        auto const result = wcsncmp(str1.data(), str2.data(), max_length);
        if(result < 0)
        {
            return result;
        }
        if(result > 0)
        {
            return result;
        }
        if(str1.length() > str2.length())
        {
            return 1;
        }
        if(str1.length() < str2.length())
        {
            return -1;
        }
        return 0;
    }

    [[nodiscard]] inline bool i_ends_with(std::wstring_view const& str1, std::wstring_view const& str2)
    {
        if(str1.length() < str2.length())
        {
            return false;
        }

        return _wcsnicmp(str1.data() + str1.length() - str2.length(), str2.data(), str2.length()) == 0;
    }
}
