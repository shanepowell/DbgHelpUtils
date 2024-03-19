#pragma once

#include "SizeNumberConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct SizeNumberConverter : SizeNumberConverterT<SizeNumberConverter>
    {
        SizeNumberConverter();

        static Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SizeNumberConverter : SizeNumberConverterT<SizeNumberConverter, implementation::SizeNumberConverter>
    {
    };
}
