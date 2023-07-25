#include "pch.h"
#include "SettingsExpanderAutomationPeer.h"

#if __has_include("SettingsExpanderAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "SettingsExpanderAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Automation;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppRedundantQualifier
    SettingsExpanderAutomationPeer::SettingsExpanderAutomationPeer(MiniDumpExplorer::SettingsExpander const& settingsExpander)
        : SettingsExpanderAutomationPeerT<SettingsExpanderAutomationPeer>(settingsExpander)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Peers::AutomationControlType SettingsExpanderAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Peers::AutomationControlType::Group;
    }

    hstring SettingsExpanderAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }

    void SettingsExpanderAutomationPeer::RaiseExpandedChangedEvent(bool const newValue) const
    {
        auto const newState = (newValue == true) ? ExpandCollapseState::Expanded : ExpandCollapseState::Collapsed;
        auto const oldState = (newState == ExpandCollapseState::Expanded) ? ExpandCollapseState::Collapsed : ExpandCollapseState::Expanded;

        RaisePropertyChangedEvent(ExpandCollapsePatternIdentifiers::ExpandCollapseStateProperty(), box_value(oldState), box_value(newState));
    }
}
