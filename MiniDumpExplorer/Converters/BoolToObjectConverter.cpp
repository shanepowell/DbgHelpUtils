#include "pch.h"
#include "BoolToObjectConverter.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Utility/ConverterTools.h"

#if __has_include("BoolToObjectConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "BoolToObjectConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    BoolToObjectConverter::BoolToObjectConverter() = default;

    DependencyProperty BoolToObjectConverter::TrueValueProperty()
    {
        static DependencyProperty s_TrueValueProperty = DependencyProperty::Register(
            L"TrueValue",
            xaml_typename<IInspectable>(),
            xaml_typename<MiniDumpExplorer::BoolToObjectConverter>(),
            PropertyMetadata{ nullptr });
        return s_TrueValueProperty;
    }

    DependencyProperty BoolToObjectConverter::FalseValueProperty()
    {
        static DependencyProperty s_FalseValueProperty = DependencyProperty::Register(
            L"FalseValue",
            xaml_typename<IInspectable>(),
            xaml_typename<MiniDumpExplorer::BoolToObjectConverter>(),
            PropertyMetadata{ nullptr });
        return s_FalseValueProperty;
    }

    Windows::Foundation::IInspectable BoolToObjectConverter::TrueValue() const
    {
        return GetValue(TrueValueProperty());
    }

    void BoolToObjectConverter::TrueValue(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(TrueValueProperty(), value);
    }

    Windows::Foundation::IInspectable BoolToObjectConverter::FalseValue() const
    {
        return GetValue(FalseValueProperty());
    }

    void BoolToObjectConverter::FalseValue(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(FalseValueProperty(), value);
    }

    Windows::Foundation::IInspectable BoolToObjectConverter::Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language) const
    {
        auto boolValue = unbox_value_or(value, false);

        // Negate if needed
        if (ConverterTools::TryParseBool(parameter))
        {
            boolValue = !boolValue;
        }

        return ConverterTools::Convert(boolValue ? TrueValue() : FalseValue(), targetType);
    }

    Windows::Foundation::IInspectable BoolToObjectConverter::ConvertBack(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language) const
    {
        auto result = value == ConverterTools::Convert(TrueValue(), { get_class_name(value) });

        if (ConverterTools::TryParseBool(parameter))
        {
            result = !result;
        }

        return box_value(result);
    }
}
