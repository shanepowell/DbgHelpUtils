#pragma once

#include "BooleanToVisibilityConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct BooleanToVisibilityConverter : BooleanToVisibilityConverterT<BooleanToVisibilityConverter>
    {
        BooleanToVisibilityConverter() = default;

        [[nodiscard]] bool IsReversed() const;
        void IsReversed(bool value);
        [[nodiscard]] Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language) const;
        [[nodiscard]] static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language);

    private:
        bool reversed_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct BooleanToVisibilityConverter : BooleanToVisibilityConverterT<BooleanToVisibilityConverter, implementation::BooleanToVisibilityConverter>
    {
    };
}
