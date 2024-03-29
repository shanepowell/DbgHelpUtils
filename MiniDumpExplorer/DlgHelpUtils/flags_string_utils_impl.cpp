#include "pch.h"
#include "DbgHelpUtils/flags_string_utils.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

namespace dlg_help_utils::flags_string_utils
{
    std::wstring get_unknown_options_string()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        return std::wstring{rm.MainResourceMap().GetValue(L"Resources/FlagsUnknownOptions").ValueAsString()};
    }

    std::wstring get_none_options_string()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        return std::wstring{rm.MainResourceMap().GetValue(L"Resources/FlagsNone").ValueAsString()};
    }
}