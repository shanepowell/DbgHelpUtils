// ReSharper disable CppHidingFunction
#pragma once

#include "SettingsExpanderAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct SettingsExpander;

    struct SettingsExpanderAutomationPeer : SettingsExpanderAutomationPeerT<SettingsExpanderAutomationPeer>
    {
        SettingsExpanderAutomationPeer(MiniDumpExplorer::SettingsExpander const& settingsExpander);
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const;
        hstring GetClassNameCore() const;
        void RaiseExpandedChangedEvent(bool newValue) const;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SettingsExpanderAutomationPeer : SettingsExpanderAutomationPeerT<SettingsExpanderAutomationPeer, implementation::SettingsExpanderAutomationPeer>
    {
    };
}
