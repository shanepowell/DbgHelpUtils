#pragma once

#include "DateTimeConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DateTimeConverter : DateTimeConverterT<DateTimeConverter>
    {
        DateTimeConverter() = default;

        [[nodiscard]] bool IsUtc() const;
        void IsUtc(bool value);

        [[nodiscard]] Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language) const;
        [[nodiscard]] static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language);

    private:
        bool utc_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DateTimeConverter : DateTimeConverterT<DateTimeConverter, implementation::DateTimeConverter>
    {
    };
}
