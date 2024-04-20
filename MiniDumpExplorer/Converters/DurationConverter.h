#pragma once

#include "DurationConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DurationConverter : DurationConverterT<DurationConverter>
    {
        DurationConverter();

        static Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DurationConverter : DurationConverterT<DurationConverter, implementation::DurationConverter>
    {
    };
}
