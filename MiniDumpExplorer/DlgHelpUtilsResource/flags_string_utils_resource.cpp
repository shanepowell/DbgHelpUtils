#include "pch.h"
#include "DbgHelpUtils/flags_string_utils.h"

#include "Utility/GetResourceValue.h"

namespace dlg_help_utils::flags_string_utils::resources
{
    std::wstring get_unknown_flags_string()
    {
        return Utility::GetResourceValue(L"Resources/FlagsUnknown");
    }

    std::wstring get_flag_separator_string()
    {
        return Utility::GetResourceValue(L"Resources/FlagsSeparator");
    }

    std::wstring get_unknown_enum_string()
    {
        return Utility::GetResourceValue(L"Resources/EnumUnknown");
    }

    std::wstring get_none_options_string()
    {
        return Utility::GetResourceValue(L"Resources/FlagsNone");
    }
}