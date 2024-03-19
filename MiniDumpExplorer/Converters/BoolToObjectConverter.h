#pragma once

#include "BoolToObjectConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct BoolToObjectConverter : BoolToObjectConverterT<BoolToObjectConverter>
    {
        BoolToObjectConverter();

        static Microsoft::UI::Xaml::DependencyProperty TrueValueProperty();
        static Microsoft::UI::Xaml::DependencyProperty FalseValueProperty();

        Windows::Foundation::IInspectable TrueValue() const;
        void TrueValue(Windows::Foundation::IInspectable const& value) const;

        Windows::Foundation::IInspectable FalseValue() const;
        void FalseValue(Windows::Foundation::IInspectable const& value) const;

        Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language) const;
        Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language) const;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct BoolToObjectConverter : BoolToObjectConverterT<BoolToObjectConverter, implementation::BoolToObjectConverter>
    {
    };
}
