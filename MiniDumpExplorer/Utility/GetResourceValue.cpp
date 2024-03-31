#include "pch.h"
#include "GetResourceValue.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

namespace Utility
{
    std::wstring GetResourceValue(std::wstring_view const resource_name)
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        return std::wstring{rm.MainResourceMap().GetValue(resource_name).ValueAsString()};
    }
}