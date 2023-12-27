#pragma once

#include "DataGridRowsPresenterAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridAutomationPeer;
    struct DataGridRowsPresenter;

    struct DataGridRowsPresenterAutomationPeer : DataGridRowsPresenterAutomationPeerT<DataGridRowsPresenterAutomationPeer>
    {
        DataGridRowsPresenterAutomationPeer(MiniDumpExplorer::DataGridRowsPresenter const& owner);

        // ReSharper disable CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const; // override
        Windows::Foundation::Collections::IVector<Microsoft::UI::Xaml::Automation::Peers::AutomationPeer> GetChildrenCore() const; // override
        hstring GetClassNameCore() const; // override
        bool IsContentElementCore() const; // override 
        // ReSharper restore CppHidingFunction

    private:
        // ReSharper disable once CppFunctionIsNotImplemented
        auto OwningRowsPresenter() const;
        MiniDumpExplorer::DataGridAutomationPeer GridPeer() const;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowsPresenterAutomationPeer : DataGridRowsPresenterAutomationPeerT<DataGridRowsPresenterAutomationPeer, implementation::DataGridRowsPresenterAutomationPeer>
    {
    };
}
