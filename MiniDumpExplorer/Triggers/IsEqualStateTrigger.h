#pragma once

#include "IsEqualStateTrigger.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct IsEqualStateTrigger : IsEqualStateTriggerT<IsEqualStateTrigger>
    {
        IsEqualStateTrigger();

        Windows::Foundation::IInspectable Value() const;
        void Value(Windows::Foundation::IInspectable const& value) const;
        static Microsoft::UI::Xaml::DependencyProperty ValueProperty();
        Windows::Foundation::IInspectable To() const;
        void To(Windows::Foundation::IInspectable const& value) const;
        static Microsoft::UI::Xaml::DependencyProperty ToProperty();

    private:
        void UpdateTrigger() const;
        static bool AreValuesEqual(Windows::Foundation::IInspectable const&  value1, Windows::Foundation::IInspectable const&  value2, bool convertType);
        static bool ConvertTypeEquals(Windows::Foundation::IInspectable const&  value1, Windows::Foundation::IInspectable const&  value2);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct IsEqualStateTrigger : IsEqualStateTriggerT<IsEqualStateTrigger, implementation::IsEqualStateTrigger>
    {
    };
}
