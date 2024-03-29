#include "flags_string_utils.h"

using namespace std::string_literals;

namespace dlg_help_utils::flags_string_utils
{
    std::wstring get_unknown_options_string()
    {
        return L"unknown options"s;
    }

    std::wstring get_none_options_string()
    {
        return L"none"s;
    }
}