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
}