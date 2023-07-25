#include "pch.h"

#include "ControlHelpers.h"

#include <winrt/Windows.Foundation.Metadata.h>

namespace ControlHelpers
{
    bool IsXamlRootAvailable()
    {
        return winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.UIElement", L"XamlRoot");
    }
}