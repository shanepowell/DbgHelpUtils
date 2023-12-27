#pragma once

#include "DataGridRowAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRow;

    struct DataGridRowAutomationPeer : DataGridRowAutomationPeerT<DataGridRowAutomationPeer>
    {
        DataGridRowAutomationPeer(MiniDumpExplorer::DataGridRow const& owner);

        // ReSharper disable CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const; // override
        hstring GetClassNameCore() const; // override
        hstring GetItemTypeCore() const; // override
        // ReSharper restore CppHidingFunction
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowAutomationPeer : DataGridRowAutomationPeerT<DataGridRowAutomationPeer, implementation::DataGridRowAutomationPeer>
    {
    };
}
