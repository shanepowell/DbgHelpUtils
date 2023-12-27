#include "pch.h"
#include "DataGridColumnHeadersPresenterAutomationPeer.h"

#if __has_include("DataGridColumnHeadersPresenterAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumnHeadersPresenterAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridColumnHeadersPresenterAutomationPeer::DataGridColumnHeadersPresenterAutomationPeer(MiniDumpExplorer::DataGridColumnHeadersPresenter const& owner)
        : base_type(owner)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType DataGridColumnHeadersPresenterAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::Header;
    }

    hstring DataGridColumnHeadersPresenterAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    bool DataGridColumnHeadersPresenterAutomationPeer::IsContentElementCore() const
    {
        return false;
    }
}
