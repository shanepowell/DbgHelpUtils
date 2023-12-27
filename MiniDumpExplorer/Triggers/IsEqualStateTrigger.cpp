#include "pch.h"
#include "IsEqualStateTrigger.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Utility/InspectableUtility.h"

#if __has_include("IsEqualStateTrigger.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "IsEqualStateTrigger.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif


using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    IsEqualStateTrigger::IsEqualStateTrigger() = default;

    Windows::Foundation::IInspectable IsEqualStateTrigger::Value() const
    {
        return GetValue(ValueProperty());
    }

    void IsEqualStateTrigger::Value(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(ValueProperty(), value);
    }

    DependencyProperty IsEqualStateTrigger::ValueProperty()
    {
        static DependencyProperty valueProperty =
            DependencyProperty::Register(
                L"Value",
                xaml_typename<Windows::Foundation::IInspectable>(),
                xaml_typename<MiniDumpExplorer::IsEqualStateTrigger>(),
                PropertyMetadata{ nullptr, [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<IsEqualStateTrigger>()->UpdateTrigger(); } });

        return valueProperty;
    }

    Windows::Foundation::IInspectable IsEqualStateTrigger::To() const
    {
        return GetValue(ToProperty());
    }

    void IsEqualStateTrigger::To(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(ToProperty(), value);
    }

    DependencyProperty IsEqualStateTrigger::ToProperty()
    {
        static DependencyProperty toProperty =
            DependencyProperty::Register(
                L"To",
                xaml_typename<Windows::Foundation::IInspectable>(),
                xaml_typename<MiniDumpExplorer::IsEqualStateTrigger>(),
                PropertyMetadata{ nullptr, [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<IsEqualStateTrigger>()->UpdateTrigger(); } });

        return toProperty;
    }

    void IsEqualStateTrigger::UpdateTrigger() const
    {
         SetActive(AreValuesEqual(Value(), To(), true));
    }

    bool IsEqualStateTrigger::AreValuesEqual(Windows::Foundation::IInspectable const& value1, Windows::Foundation::IInspectable const& value2, bool const convertType)
    {
        if (value1 == value2)
        {
            return true;
        }

        // If they are the same type but fail with Equals check, don't bother with conversion.
        if (value1 != nullptr && value2 != nullptr && convertType)
        {
            // Try the conversion in both ways:
            return ConvertTypeEquals(value1, value2);
        }

        return false;
    }

    bool IsEqualStateTrigger::ConvertTypeEquals(Windows::Foundation::IInspectable const& value1, Windows::Foundation::IInspectable const& value2)
    {
        if(InspectableUtility::Equals<bool>(value1, value2)
            || InspectableUtility::Equals<hstring>(value1, value2)
            || InspectableUtility::Equals<GUID>(value1, value2)
            || InspectableUtility::EqualsIntegral(value1, value2)
            || InspectableUtility::EqualsFloat(value1, value2)
        )
        {
            return true;
        }
        return false;
    }
}
