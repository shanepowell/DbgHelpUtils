#include "flags_string_utils.h"

using namespace std::string_literals;

namespace dlg_help_utils::flags_string_utils::resources
{
    std::wstring get_unknown_flags_string()
    {
        return L"unknown"s;
    }

    std::wstring get_flag_separator_string()
    {
        return L","s;
    }

    std::wstring get_unknown_enum_string()
    {
        return L"unknown"s;
    }

    std::wstring get_none_options_string()
    {
        return L"none"s;
    }
}