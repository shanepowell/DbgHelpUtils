#pragma once

#include "AtLeastToVisibilityConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct AtLeastToVisibilityConverter : AtLeastToVisibilityConverterT<AtLeastToVisibilityConverter>
    {
        AtLeastToVisibilityConverter() = default;

        [[nodiscard]] bool IsReversed() const;
        void IsReversed(bool value);
        [[nodiscard]] hstring Value() const;
        void Value(hstring value);

        [[nodiscard]] Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language) const;
        [[nodiscard]] static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language);

    private:
        bool reversed_{false};
        hstring test_value_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct AtLeastToVisibilityConverter : AtLeastToVisibilityConverterT<AtLeastToVisibilityConverter, implementation::AtLeastToVisibilityConverter>
    {
    };
}
