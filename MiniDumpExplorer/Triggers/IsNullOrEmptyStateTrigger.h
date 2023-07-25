#pragma once

#include "IsNullOrEmptyStateTrigger.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct IsNullOrEmptyStateTrigger : IsNullOrEmptyStateTriggerT<IsNullOrEmptyStateTrigger>
    {
        IsNullOrEmptyStateTrigger();

        Windows::Foundation::IInspectable Value() const;
        void Value(Windows::Foundation::IInspectable const& value) const;
        static Microsoft::UI::Xaml::DependencyProperty ValueProperty();

    private:
        void OnValuePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& args);
        static bool IsNullOrEmpty(Windows::Foundation::IInspectable const& value);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct IsNullOrEmptyStateTrigger : IsNullOrEmptyStateTriggerT<IsNullOrEmptyStateTrigger, implementation::IsNullOrEmptyStateTrigger>
    {
    };
}
