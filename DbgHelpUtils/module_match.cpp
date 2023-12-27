#include "module_match.h"

#include <filesystem>

#include "stream_module_name.h"
#include "string_compare.h"

namespace dlg_help_utils::module_match
{
    bool module_name_match(stream_module_name const& key, std::wstring_view const& module_name)
    {
        return string_utils::iequals(key.filename_with_extension(), module_name) || string_utils::iequals(key.filename_without_extension(), module_name);
    }
}
