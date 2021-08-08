#include "module_match.h"

#include <filesystem>

#include "string_compare.h"

namespace dlg_help_utils::module_match
{
    bool module_name_match(std::wstring_view const& key, std::wstring_view const& module_name)
    {
        std::filesystem::path const path{key};
        auto const filename_with_extension = path.filename();
        auto const filename_without_extension = std::filesystem::path{filename_with_extension}.replace_extension();
        return string_compare::iequals(filename_with_extension.wstring(), module_name) || string_compare::iequals(filename_without_extension.wstring(), module_name);
    }
}
