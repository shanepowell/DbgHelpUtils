#pragma once

#include "DataGridRowHeaderAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRowHeader;

    struct DataGridRowHeaderAutomationPeer : DataGridRowHeaderAutomationPeerT<DataGridRowHeaderAutomationPeer>
    {
        DataGridRowHeaderAutomationPeer(MiniDumpExplorer::DataGridRowHeader const& owner);

        // ReSharper disable CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const; // override
        hstring GetClassNameCore() const; // override
        hstring GetNameCore() const; // override
        // ReSharper disable once CppMemberFunctionMayBeStatic
        bool IsContentElementCore() const { return false; } // override
        // ReSharper restore CppHidingFunction

    private:
        MiniDumpExplorer::DataGridRowHeader OwningHeader() const { return Owner().as<MiniDumpExplorer::DataGridRowHeader>(); }
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowHeaderAutomationPeer : DataGridRowHeaderAutomationPeerT<DataGridRowHeaderAutomationPeer, implementation::DataGridRowHeaderAutomationPeer>
    {
    };
}
