#pragma once

#include "SettingsExpanderItemStyleSelector.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct SettingsExpanderItemStyleSelector : SettingsExpanderItemStyleSelectorT<SettingsExpanderItemStyleSelector>
    {
        SettingsExpanderItemStyleSelector();

        Microsoft::UI::Xaml::Style DefaultStyle() const;
        void DefaultStyle(Microsoft::UI::Xaml::Style const& value);
        Microsoft::UI::Xaml::Style ClickableStyle() const;
        void ClickableStyle(Microsoft::UI::Xaml::Style const& value);

        Microsoft::UI::Xaml::Style SelectStyleCore(Windows::Foundation::IInspectable const& item, Microsoft::UI::Xaml::DependencyObject const& container);

    private:
        Microsoft::UI::Xaml::Style defaultStyle_{nullptr};
        Microsoft::UI::Xaml::Style clickableStyle_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SettingsExpanderItemStyleSelector : SettingsExpanderItemStyleSelectorT<SettingsExpanderItemStyleSelector, implementation::SettingsExpanderItemStyleSelector>
    {
    };
}
