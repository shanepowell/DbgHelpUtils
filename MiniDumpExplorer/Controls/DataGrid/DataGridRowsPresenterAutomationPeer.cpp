#include "pch.h"
#include "DataGridRowsPresenterAutomationPeer.h"

#include "DataGridRowsPresenter.h"
#include "DataGridAutomationPeer.h"

#if __has_include("DataGridRowsPresenterAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowsPresenterAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    auto DataGridRowsPresenterAutomationPeer::OwningRowsPresenter() const
    {
        return Owner().try_as<DataGridRowsPresenter>();
    }

    DataGridRowsPresenterAutomationPeer::DataGridRowsPresenterAutomationPeer(MiniDumpExplorer::DataGridRowsPresenter const& owner)
        : base_type(owner)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType DataGridRowsPresenterAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::Custom;
    }

    Windows::Foundation::Collections::IVector<Automation::Peers::AutomationPeer> DataGridRowsPresenterAutomationPeer::GetChildrenCore() const
    {
        auto const gridPeer = GridPeer().try_as<DataGridAutomationPeer>();
        if (!gridPeer)
        {
            return single_threaded_vector<Automation::Peers::AutomationPeer>();
        }

        return gridPeer->GetChildPeers();
    }

    hstring DataGridRowsPresenterAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    bool DataGridRowsPresenterAutomationPeer::IsContentElementCore() const
    {
        return false;
    }

    MiniDumpExplorer::DataGridAutomationPeer DataGridRowsPresenterAutomationPeer::GridPeer() const
    {
        if (auto const owningGrid = OwningRowsPresenter()->OwningGrid();
            owningGrid)
        {
            return Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(owningGrid).try_as<MiniDumpExplorer::DataGridAutomationPeer>();
        }

        return nullptr;
    }
}
