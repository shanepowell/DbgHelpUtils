#pragma once

#include "DataGridColumnHeadersPresenter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridColumn;
    struct DataGridColumnHeader;

    struct DataGridColumnHeadersPresenter : DataGridColumnHeadersPresenterT<DataGridColumnHeadersPresenter>
    {
        DataGridColumnHeadersPresenter();

        MiniDumpExplorer::DataGridColumn const& DragColumn() { return dragColumn_; }
        void DragColumn(MiniDumpExplorer::DataGridColumn const& value) { dragColumn_ = value; }
        Microsoft::UI::Xaml::Controls::Control DragIndicator() const { return dragIndicator_; }
        void DragIndicator(Microsoft::UI::Xaml::Controls::Control const& value);
        double DragIndicatorOffset() const { return dragIndicatorOffset_; }
        void DragIndicatorOffset(double const value) { dragIndicatorOffset_ = value; }
        Microsoft::UI::Xaml::Controls::Control DropLocationIndicator() const { return dropLocationIndicator_; }
        void DropLocationIndicator(Microsoft::UI::Xaml::Controls::Control const& value);
        double DropLocationIndicatorOffset() const { return dropLocationIndicatorOffset_; }
        void DropLocationIndicatorOffset(double const value) { dropLocationIndicatorOffset_ = value; }
        MiniDumpExplorer::DataGrid const& OwningGrid() { return owningGrid_; }
        void OwningGrid(MiniDumpExplorer::DataGrid const& value) { owningGrid_ = value; }

        // ReSharper disable CppHidingFunction
        Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size const& finalSize); // override
        Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const& availableSize); // override
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const; // override
        // ReSharper restore CppHidingFunction

    private:
        static void EnsureColumnHeaderClip(MiniDumpExplorer::DataGridColumnHeader const& columnHeader, double width, double height, double frozenLeftEdge, double columnHeaderLeftEdge);

        void EnsureColumnReorderingClip(Microsoft::UI::Xaml::Controls::Control const& control, double height, double frozenColumnsWidth, double controlLeftEdge);

        // ReSharper disable once CppFunctionIsNotImplemented
        auto InternalOwningGrid();

    private:
        MiniDumpExplorer::DataGridColumn dragColumn_{nullptr};
        Microsoft::UI::Xaml::Controls::Control dragIndicator_{nullptr};
        double dragIndicatorOffset_{};
        Microsoft::UI::Xaml::Controls::Control dropLocationIndicator_{nullptr};
        double dropLocationIndicatorOffset_{};
        MiniDumpExplorer::DataGrid owningGrid_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridColumnHeadersPresenter : DataGridColumnHeadersPresenterT<DataGridColumnHeadersPresenter, implementation::DataGridColumnHeadersPresenter>
    {
    };
}
