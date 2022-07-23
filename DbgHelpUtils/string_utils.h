#pragma once
#include <string>

namespace dlg_help_utils::string_compare
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
}