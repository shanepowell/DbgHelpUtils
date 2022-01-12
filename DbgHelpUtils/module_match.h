#pragma once
#include <string_view>

namespace dlg_help_utils
{
    class stream_module_name;
}

namespace dlg_help_utils::module_match
{
    [[nodiscard]] bool module_name_match(stream_module_name const& key, std::wstring_view const& module_name);
}
