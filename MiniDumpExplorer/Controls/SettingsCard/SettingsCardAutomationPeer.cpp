#include "pch.h"
#include "SettingsCardAutomationPeer.h"

#include "App.xaml.h"

#if __has_include("SettingsCardAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "SettingsCardAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    SettingsCardAutomationPeer::SettingsCardAutomationPeer(MiniDumpExplorer::SettingsCard const& settingsCard)
        : SettingsCardAutomationPeerT<SettingsCardAutomationPeer>(settingsCard)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType SettingsCardAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::Group;
    }

    hstring SettingsCardAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }
}
