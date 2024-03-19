#pragma once

#include "TimeStampNumberConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct TimeStampNumberConverter : TimeStampNumberConverterT<TimeStampNumberConverter>
    {
        TimeStampNumberConverter();

        static Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct TimeStampNumberConverter : TimeStampNumberConverterT<TimeStampNumberConverter, implementation::TimeStampNumberConverter>
    {
    };
}
