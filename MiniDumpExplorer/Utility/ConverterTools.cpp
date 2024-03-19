#include "pch.h"
#include "ConverterTools.h"

#include<winrt/Windows.Foundation.h>

#include "DbgHelpUtils/string_compare.h"

bool ConverterTools::TryParseBool(Windows::Foundation::IInspectable const& parameter)
{
    auto parsed = false;
    if (parameter)
    {
        auto const value = unbox_value_or(parameter, hstring{});
        parsed = dlg_help_utils::string_utils::iequals(value, L"true"sv);
    }

    return parsed;
}

Windows::Foundation::IInspectable ConverterTools::Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType)
{
    if (targetType.Name == get_class_name(value))
    {
        return value;
    }

    return Microsoft::UI::Xaml::Markup::XamlBindingHelper::ConvertValue(targetType, value);
}
