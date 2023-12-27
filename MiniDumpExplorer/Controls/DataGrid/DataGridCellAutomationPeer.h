#pragma once

#include "DataGridCellAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridCell;
    struct DataGridColumn;
    struct DataGridRow;

    struct DataGridCellAutomationPeer : DataGridCellAutomationPeerT<DataGridCellAutomationPeer>
    {
        DataGridCellAutomationPeer(MiniDumpExplorer::DataGridCell const& owner);

        Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple ContainingGrid() const;
        // ReSharper disable once CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const;
        // ReSharper disable once CppHidingFunction
        hstring GetClassNameCore() const;
        // ReSharper disable once CppHidingFunction
        hstring GetNameCore() const;
        Windows::Foundation::IInspectable GetPatternCore(Microsoft::UI::Xaml::Automation::Peers::PatternInterface patternInterface) const; // override
        static bool IsKeyboardFocusableCore() { return true; }

        // IGridItemProvider
        int32_t Column() const;
        static int32_t ColumnSpan() { return 1; }
        int32_t Row() const;
        static int32_t RowSpan() { return 1; }

        // IInvokeProvider
        void Invoke();

        // IScrollItemProvider
        void ScrollIntoView() const;

        // ISelectionItemProvider
        bool IsSelected() const;
        Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple SelectionContainer() const;
        void AddToSelection() const;
        void RemoveFromSelection() const;
        void Select();

        // ITableItemProvider
        com_array<Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple> GetColumnHeaderItems() const;
        com_array<Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple> GetRowHeaderItems() const;

    private:
        MiniDumpExplorer::DataGridCell OwningCell() const { return Owner().try_as<MiniDumpExplorer::DataGridCell>(); }
        MiniDumpExplorer::DataGridColumn OwningColumn() const;
        MiniDumpExplorer::DataGrid OwningGrid() const;
        MiniDumpExplorer::DataGridRow OwningRow() const;

        // ReSharper disable CppFunctionIsNotImplemented
        auto InternalOwningCell() const;
        auto InternalOwningColumn() const;
        auto TryInternalOwningColumn() const;
        auto TryInternalOwningGrid() const;
        auto InternalOwningRow() const;
        auto TryInternalOwningRow() const;
        // ReSharper restore CppFunctionIsNotImplemented

        void EnsureEnabled() const;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridCellAutomationPeer : DataGridCellAutomationPeerT<DataGridCellAutomationPeer, implementation::DataGridCellAutomationPeer>
    {
    };
}
