#pragma once

#include "HexNumberConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct HexNumberConverter : HexNumberConverterT<HexNumberConverter>
    {
        HexNumberConverter();

        static Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct HexNumberConverter : HexNumberConverterT<HexNumberConverter, implementation::HexNumberConverter>
    {
    };
}
