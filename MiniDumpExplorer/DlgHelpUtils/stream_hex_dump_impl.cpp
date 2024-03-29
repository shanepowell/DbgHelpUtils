#include "pch.h"
#include "DbgHelpUtils/stream_hex_dump.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

using namespace std::string_view_literals;

namespace dlg_help_utils::stream_hex_dump
{
    std::wstring get_hex_number_prefix()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        return std::wstring{rm.MainResourceMap().GetValue(L"Resources/HexNumberPrefix").ValueAsString()};
    }
}
