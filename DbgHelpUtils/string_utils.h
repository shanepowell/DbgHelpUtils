#pragma once
#include <string>

namespace dlg_help_utils::string_utils
{
    [[nodiscard]] inline std::wstring to_lower(std::wstring_view const& str)
    {
        std::wstring rv{str};
        std::ignore = _wcslwr_s(rv.data(), rv.size() + 1);
        return rv;
    }

    [[nodiscard]] inline std::wstring to_upper(std::wstring_view const& str)
    {
        std::wstring rv{str};
        std::ignore = _wcsupr_s(rv.data(), rv.size() + 1);
        return rv;
    }

    [[nodiscard]] inline std::wstring_view trim_start(wchar_t const ch, std::wstring_view str)
    {
        if(auto const position = str.find_first_not_of(ch); position != std::wstring_view::npos)
        {
            str = str.substr(position);
        }

        return str;
    }

    [[nodiscard]] inline std::wstring_view trim_end(wchar_t const ch, std::wstring_view str)
    {
        if(auto const position = str.find_last_not_of(ch); position != std::wstring_view::npos)
        {
            str = str.substr(0, position + 1);
        }

        return str;
    }

    [[nodiscard]] inline std::wstring_view trim(wchar_t const ch, std::wstring_view const& str)
    {
        return trim_end(ch, trim_start(ch, str));
    }

    inline void indent_string(std::wstring& str, size_t const indent, wchar_t const ch = L' ')
    {
        if(str.empty() || indent == 0)
        {
            return;
        }

        std::wstring indentStr(indent, ch);
        str.insert(0, indentStr);

        auto position = str.find_first_of('\n', indent);
        while(position != std::wstring::npos && position < str.size() - 1)
        {
            str.insert(position + 1, indentStr);
            position = str.find_first_of('\n', position + 1 + indent);
        }
    }
}