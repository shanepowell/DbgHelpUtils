#pragma once

#include "DataGridColumnHeadersPresenterAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumnHeadersPresenter;

    struct DataGridColumnHeadersPresenterAutomationPeer : DataGridColumnHeadersPresenterAutomationPeerT<DataGridColumnHeadersPresenterAutomationPeer>
    {
        DataGridColumnHeadersPresenterAutomationPeer(MiniDumpExplorer::DataGridColumnHeadersPresenter const& owner);

        // ReSharper disable CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const; // override
        hstring GetClassNameCore() const; // override
        bool IsContentElementCore() const; // override 
        // ReSharper restore CppHidingFunction
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridColumnHeadersPresenterAutomationPeer : DataGridColumnHeadersPresenterAutomationPeerT<DataGridColumnHeadersPresenterAutomationPeer, implementation::DataGridColumnHeadersPresenterAutomationPeer>
    {
    };
}
