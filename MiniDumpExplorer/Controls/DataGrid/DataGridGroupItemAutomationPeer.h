#pragma once

#include "DataGridGroupItemAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridAutomationPeer;
    struct DataGridRowGroupHeader;
    struct DataGridRowGroupHeaderAutomationPeer;

    struct DataGridGroupItemAutomationPeer : DataGridGroupItemAutomationPeerT<DataGridGroupItemAutomationPeer>
    {
        DataGridGroupItemAutomationPeer(Microsoft::UI::Xaml::Data::ICollectionViewGroup const& group, MiniDumpExplorer::DataGrid const& dataGrid);

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

        // IExpandCollapseProvider
        void Collapse() const;
        void Expand() const;
        Microsoft::UI::Xaml::Automation::ExpandCollapseState ExpandCollapseState() const;

        // IGridProvider
        int32_t ColumnCount() const;
        int32_t RowCount() const;
        Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple GetItem(int32_t row, int32_t column) const;

        // IScrollItemProvider
        void ScrollIntoView() const;

        // ISelectionProvider
        com_array<Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple> GetSelection() const;
        bool CanSelectMultiple() const;
        bool IsSelectionRequired() const;

        MiniDumpExplorer::DataGridRowGroupHeaderAutomationPeer OwningRowGroupHeaderPeer() const;

    private:
        MiniDumpExplorer::DataGrid OwningDataGrid() const;
        MiniDumpExplorer::DataGridAutomationPeer OwningDataGridPeer() const;
        MiniDumpExplorer::DataGridRowGroupHeader OwningRowGroupHeader() const;

        void EnsureEnabled() const;

    private:
        Microsoft::UI::Xaml::Data::ICollectionViewGroup group_;
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer dataGridAutomationPeer_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridGroupItemAutomationPeer : DataGridGroupItemAutomationPeerT<DataGridGroupItemAutomationPeer, implementation::DataGridGroupItemAutomationPeer>
    {
    };
}
