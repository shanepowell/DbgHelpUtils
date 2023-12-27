#include "pch.h"
#include "SettingsExpanderItemStyleSelector.h"
#if __has_include("SettingsExpanderItemStyleSelector.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "SettingsExpanderItemStyleSelector.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    SettingsExpanderItemStyleSelector::SettingsExpanderItemStyleSelector() = default;

    Style SettingsExpanderItemStyleSelector::DefaultStyle() const
    {
        return defaultStyle_;
    }

    void SettingsExpanderItemStyleSelector::DefaultStyle(Style const& value)
    {
        defaultStyle_ = value;
    }

    Style SettingsExpanderItemStyleSelector::ClickableStyle() const
    {
        return clickableStyle_;
    }

    void SettingsExpanderItemStyleSelector::ClickableStyle(Style const& value)
    {
        clickableStyle_ = value;
    }

    Style SettingsExpanderItemStyleSelector::SelectStyleCore(Windows::Foundation::IInspectable const& item, [[maybe_unused]] DependencyObject const& container)
    {
        if (auto const card = item.try_as<SettingsCard>(); card && card.IsClickEnabled())
        {
            return clickableStyle_;
        }

        return defaultStyle_;
    }
}
