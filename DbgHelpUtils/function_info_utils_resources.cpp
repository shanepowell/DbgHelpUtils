#include "function_info_utils.h"

#include "flags_string_utils.h"

using namespace std::string_literals;

namespace 
{
    using dlg_help_utils::function_table_entry_type;

    std::unordered_map<function_table_entry_type, std::wstring> g_function_table_entry_types =
    {
        {function_table_entry_type::unknown,                    L"unknown"s},
        {function_table_entry_type::fpo_data,                   L"fpo_data"s},
        {function_table_entry_type::image_function_entry,       L"image function entry"s},
        {function_table_entry_type::image_function_entry_64,    L"image function 64 entry"s},
    };
}

namespace dlg_help_utils::function_info_utils
{
    std::wstring function_entry_type_to_string(function_table_entry_type const type)
    {
        return flags_string_utils::generate_enum_string(type, g_function_table_entry_types);
    }
}
