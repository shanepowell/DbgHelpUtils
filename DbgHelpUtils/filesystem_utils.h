#pragma once
#include <functional>
#include <string_view>
#include <vector>
#include <experimental/generator>

namespace dlg_help_utils::filesystem_utils
{
    [[nodiscard]] bool wildcard_match(std::wstring_view value, std::wstring_view match);
    [[nodiscard]] std::experimental::generator<std::wstring> enumerate_files(std::vector<std::wstring> const& paths, std::function<bool(std::wstring)> on_error);
}
