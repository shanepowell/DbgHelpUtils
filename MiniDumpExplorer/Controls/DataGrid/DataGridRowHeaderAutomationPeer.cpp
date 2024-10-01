#include "pch.h"
#include "DataGridRowHeaderAutomationPeer.h"
#if __has_include("DataGridRowHeaderAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowHeaderAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridRowHeaderAutomationPeer::DataGridRowHeaderAutomationPeer(MiniDumpExplorer::DataGridRowHeader const& owner)
        : base_type(owner)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType DataGridRowHeaderAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::HeaderItem;
    }

    hstring DataGridRowHeaderAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }

    hstring DataGridRowHeaderAutomationPeer::GetNameCore() const
    {
        if(auto const content = OwningHeader().Content().try_as<hstring>();
            content.has_value())
        {
            return to_hstring(content.value());
        }
        return base_type::GetNameCore();
    }
}
