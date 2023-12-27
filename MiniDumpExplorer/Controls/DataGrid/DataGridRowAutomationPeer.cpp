#include "pch.h"
#include "DataGridRowAutomationPeer.h"
#if __has_include("DataGridRowAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridRowAutomationPeer::DataGridRowAutomationPeer(MiniDumpExplorer::DataGridRow const& owner)
        : base_type(owner)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType DataGridRowAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::DataItem;
    }

    hstring DataGridRowAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }

    hstring DataGridRowAutomationPeer::GetItemTypeCore() const
    {
        if (auto itemType = base_type::GetItemTypeCore();
            !itemType.empty())
        {
            return itemType;
        }

        return L"row";
    }
}
