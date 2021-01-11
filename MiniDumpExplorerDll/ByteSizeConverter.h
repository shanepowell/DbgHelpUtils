#pragma once

#include "ByteSizeConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ByteSizeConverter : ByteSizeConverterT<ByteSizeConverter>
    {
        ByteSizeConverter() = default;

        [[nodiscard]] Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language) const;
        [[nodiscard]] static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ByteSizeConverter : ByteSizeConverterT<ByteSizeConverter, implementation::ByteSizeConverter>
    {
    };
}
