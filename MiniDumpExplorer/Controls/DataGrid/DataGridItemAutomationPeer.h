#pragma once

#include "DataGridItemAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridRow;
    struct DataGridRowAutomationPeer;

    struct DataGridItemAutomationPeer : DataGridItemAutomationPeerT<DataGridItemAutomationPeer>
    {
        DataGridItemAutomationPeer(Windows::Foundation::IInspectable const& item, MiniDumpExplorer::DataGrid const& dataGrid);

        MiniDumpExplorer::DataGridRowAutomationPeer OwningRowPeer() const;

        // ReSharper disable CppHidingFunction
        hstring GetAcceleratorKeyCore() const; // override
        hstring GetAccessKeyCore() const; // override

        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const; // override

        hstring GetAutomationIdCore() const; // override
        Windows::Foundation::Rect GetBoundingRectangleCore() const; // override

        Windows::Foundation::Collections::IVector<Microsoft::UI::Xaml::Automation::Peers::AutomationPeer> GetChildrenCore() const; // override

        hstring GetClassNameCore() const; // override
        Windows::Foundation::Point GetClickablePointCore() const; // override
        hstring GetHelpTextCore() const; // override
        hstring GetItemStatusCore() const; // override
        hstring GetItemTypeCore() const; // override
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer GetLabeledByCore() const; // override
        hstring GetLocalizedControlTypeCore() const; // override
        hstring GetNameCore() const; // override
        Microsoft::UI::Xaml::Automation::Peers::AutomationOrientation GetOrientationCore() const; // override
        Windows::Foundation::IInspectable GetPatternCore(Microsoft::UI::Xaml::Automation::Peers::PatternInterface patternInterface) const; // override
        bool HasKeyboardFocusCore() const; // override
        bool IsContentElementCore() const; // override
        bool IsControlElementCore() const; // override
        bool IsEnabledCore() const; // override
        bool IsKeyboardFocusableCore() const; // override
        bool IsOffscreenCore() const; // override
        bool IsPasswordCore() const; // override
        bool IsRequiredForFormCore() const; // override
        void SetFocusCore() const; // override
        // ReSharper restore CppHidingFunction

        // IInvokeProvider
        void Invoke();

        // IScrollItemProvider
        void ScrollIntoView() const;

        // ISelectionItemProvider
        bool IsSelected() const;
        Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple SelectionContainer() const;
        void AddToSelection() const;
        void RemoveFromSelection();
        void Select();

        // ISelectionProvider
        com_array<Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple> GetSelection() const;
        bool CanSelectMultiple() const;
        bool IsSelectionRequired() const;

    private:
        MiniDumpExplorer::DataGrid OwningDataGrid() const;
        MiniDumpExplorer::DataGridRow OwningRow() const;

        void EnsureEnabled() const;

    private:
        Windows::Foundation::IInspectable item_;
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer dataGridAutomationPeer_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridItemAutomationPeer : DataGridItemAutomationPeerT<DataGridItemAutomationPeer, implementation::DataGridItemAutomationPeer>
    {
    };
}
