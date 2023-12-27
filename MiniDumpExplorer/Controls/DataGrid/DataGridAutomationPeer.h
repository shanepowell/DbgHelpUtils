#pragma once

#include "DataGridAutomationPeer.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridGroupItemAutomationPeer;
    struct DataGridItemAutomationPeer;
    struct DataGridRowGroupHeader;
    struct DataGridColumn;
    struct DataGridRow;

    struct DataGridAutomationPeer : DataGridAutomationPeerT<DataGridAutomationPeer>
    {
        DataGridAutomationPeer(MiniDumpExplorer::DataGrid const& owner);

        // ReSharper disable CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const; // override
        Windows::Foundation::Collections::IVector<Microsoft::UI::Xaml::Automation::Peers::AutomationPeer> GetChildrenCore() const; // override
        hstring GetClassNameCore() const; // override
        hstring GetNameCore() const; // override
        // ReSharper restore CppHidingFunction
        Windows::Foundation::IInspectable GetPatternCore(Microsoft::UI::Xaml::Automation::Peers::PatternInterface patternInterface) const; // override

        // IGridProvider
        int32_t ColumnCount() const;
        int32_t RowCount() const;
        Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple GetItem(int32_t row, int32_t column) const;

        // IScrollProvider
        bool HorizontallyScrollable() const;
        double HorizontalScrollPercent() const;
        double HorizontalViewSize() const;
        bool VerticallyScrollable() const;
        double VerticalScrollPercent() const;
        double VerticalViewSize() const;
        void Scroll(Microsoft::UI::Xaml::Automation::ScrollAmount const& horizontalAmount, Microsoft::UI::Xaml::Automation::ScrollAmount const& verticalAmount) const;
        void SetScrollPercent(double horizontalPercent, double verticalPercent) const;

        // ISelectionProvider
        com_array<Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple> GetSelection();
        bool CanSelectMultiple() const;
        bool IsSelectionRequired() const;

        // ITableProvider
        Microsoft::UI::Xaml::Automation::RowOrColumnMajor RowOrColumnMajor() const;
        com_array<Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple> GetColumnHeaders() const;
        com_array<Microsoft::UI::Xaml::Automation::Provider::IRawElementProviderSimple> GetRowHeaders() const;

        static void RaiseAutomationInvokeEvent(Microsoft::UI::Xaml::UIElement const& element);
        Windows::Foundation::Collections::IVector<Microsoft::UI::Xaml::Automation::Peers::AutomationPeer> GetChildPeers();
        MiniDumpExplorer::DataGridGroupItemAutomationPeer GetOrCreateGroupItemPeer(Windows::Foundation::IInspectable const& group);
        MiniDumpExplorer::DataGridItemAutomationPeer GetOrCreateItemPeer(Windows::Foundation::IInspectable const& item);
        void PopulateGroupItemPeers();
        void PopulateItemPeers();
        void RaiseAutomationCellSelectedEvent(int32_t slot, int32_t column) const;
        void RaiseAutomationFocusChangedEvent(int32_t slot, int32_t column) const;
        void RaiseAutomationInvokeEvents(DataGridEditingUnit const& editingUnit, MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row);
        void RaiseAutomationScrollEvents();
        void RaiseAutomationSelectionEvents(Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void UpdateRowGroupHeaderPeerEventsSource(MiniDumpExplorer::DataGridRowGroupHeader const& header);
        void UpdateRowPeerEventsSource(MiniDumpExplorer::DataGridRow const& row);

    private:
        MiniDumpExplorer::DataGrid OwningDataGrid() const;

        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer GetCellPeer(int32_t slot, int32_t column) const;

    private:
        std::unordered_map<Windows::Foundation::IInspectable, MiniDumpExplorer::DataGridGroupItemAutomationPeer> groupItemPeers_;
        std::unordered_map<Windows::Foundation::IInspectable, MiniDumpExplorer::DataGridItemAutomationPeer> itemPeers_;
        bool oldHorizontallyScrollable_;
        double oldHorizontalScrollPercent_;
        double oldHorizontalViewSize_;
        bool oldVerticallyScrollable_;
        double oldVerticalScrollPercent_;
        double oldVerticalViewSize_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridAutomationPeer : DataGridAutomationPeerT<DataGridAutomationPeer, implementation::DataGridAutomationPeer>
    {
    };
}
