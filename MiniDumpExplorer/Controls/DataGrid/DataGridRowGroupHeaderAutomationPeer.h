#pragma once

#include "DataGridRowGroupHeaderAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRowGroupHeader;

    struct DataGridRowGroupHeaderAutomationPeer : DataGridRowGroupHeaderAutomationPeerT<DataGridRowGroupHeaderAutomationPeer>
    {
        DataGridRowGroupHeaderAutomationPeer(MiniDumpExplorer::DataGridRowGroupHeader const& owner);

        // ReSharper disable once CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const; // override
        // ReSharper disable once CppHidingFunction
        hstring GetClassNameCore() const; // override
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowGroupHeaderAutomationPeer : DataGridRowGroupHeaderAutomationPeerT<DataGridRowGroupHeaderAutomationPeer, implementation::DataGridRowGroupHeaderAutomationPeer>
    {
    };
}
