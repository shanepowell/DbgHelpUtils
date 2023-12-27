// ReSharper disable CppHidingFunction
#pragma once

#include "SettingsCardAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct SettingsCard;

    struct SettingsCardAutomationPeer : SettingsCardAutomationPeerT<SettingsCardAutomationPeer>
    {
        SettingsCardAutomationPeer(MiniDumpExplorer::SettingsCard const& settingsCard);
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const;
        hstring GetClassNameCore() const;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SettingsCardAutomationPeer : SettingsCardAutomationPeerT<SettingsCardAutomationPeer, implementation::SettingsCardAutomationPeer>
    {
    };
}
