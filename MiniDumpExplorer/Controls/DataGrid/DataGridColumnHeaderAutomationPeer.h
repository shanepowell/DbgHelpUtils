#pragma once

#include "DataGridColumnHeaderAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumnHeader;

    struct DataGridColumnHeaderAutomationPeer : DataGridColumnHeaderAutomationPeerT<DataGridColumnHeaderAutomationPeer>
    {
        DataGridColumnHeaderAutomationPeer(MiniDumpExplorer::DataGridColumnHeader const& owner);

        // ReSharper disable CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const; // override
        hstring GetClassNameCore() const; // override
        hstring GetHelpTextCore() const; // override
        hstring GetNameCore() const; // override
        Windows::Foundation::IInspectable GetPatternCore(Microsoft::UI::Xaml::Automation::Peers::PatternInterface patternInterface) const; // override

        // ReSharper disable once CppMemberFunctionMayBeStatic
        bool IsContentElementCore() const { return false; } // override
        // ReSharper restore CppHidingFunction

        // IInvokeProvider
        void Invoke() const;

        // IScrollItemProvider
        void ScrollIntoView() const;

        // ITransformProvider
        // ReSharper disable once CppMemberFunctionMayBeStatic
        bool CanMove() const { return false; }
        bool CanResize() const;
        // ReSharper disable once CppMemberFunctionMayBeStatic
        bool CanRotate() { return false; }
        void Move(double x, double y) const;
        void Resize(double width, double height) const;
        void Rotate(double degrees) const;

    private:
        MiniDumpExplorer::DataGridColumnHeader OwningHeader() const;
        // ReSharper disable once CppFunctionIsNotImplemented
        auto InternalOwningHeader() const;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridColumnHeaderAutomationPeer : DataGridColumnHeaderAutomationPeerT<DataGridColumnHeaderAutomationPeer, implementation::DataGridColumnHeaderAutomationPeer>
    {
    };
}
