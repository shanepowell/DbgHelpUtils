#pragma once
#include <string>

namespace dlg_help_utils::module_match
{
    [[nodiscard]] bool module_name_match(std::wstring_view const& key, std::wstring_view const& module_name);
}
