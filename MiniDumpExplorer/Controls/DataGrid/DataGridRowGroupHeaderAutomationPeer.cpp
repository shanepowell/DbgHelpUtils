#include "pch.h"
#include "DataGridRowGroupHeaderAutomationPeer.h"


#if __has_include("DataGridRowGroupHeaderAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowGroupHeaderAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridRowGroupHeaderAutomationPeer::DataGridRowGroupHeaderAutomationPeer(MiniDumpExplorer::DataGridRowGroupHeader const& owner)
        : base_type(owner)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType DataGridRowGroupHeaderAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::Group;
    }

    hstring DataGridRowGroupHeaderAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }
}
