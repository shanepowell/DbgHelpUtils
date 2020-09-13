#pragma once
#include <string_view>

namespace dlg_help_utils::filesystem_utils
{
    bool wildcard_match(std::wstring_view value, std::wstring_view match);
}
