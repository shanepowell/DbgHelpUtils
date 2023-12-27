#pragma once

#include "DataGridCellsPresenter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridCell;
    struct DataGridRow;

    struct DataGridCellsPresenter : DataGridCellsPresenterT<DataGridCellsPresenter>
    {
        DataGridCellsPresenter();

        MiniDumpExplorer::DataGridRow const& OwningRow() const { return owningRow_; }
        void OwningRow(MiniDumpExplorer::DataGridRow const& value) { owningRow_ = value; }

        Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size const& finalSize); // override
        Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const& availableSize); // override

        void EnsureFillerVisibility() const;
        void Recycle();

    private:
        // ReSharper disable CppFunctionIsNotImplemented
        auto InternalOwningGrid() const;
        auto InternalOwningRow() const;
        // ReSharper restore CppFunctionIsNotImplemented

        MiniDumpExplorer::DataGrid OwningGrid() const;

        bool ShouldDisplayCell(DataGridColumn const& column, double frozenLeftEdge, double scrollingLeftEdge) const;

        static void EnsureCellClip(MiniDumpExplorer::DataGridCell const& cell, double width, double height, double frozenLeftEdge, double cellLeftEdge);
        static void EnsureCellDisplay(MiniDumpExplorer::DataGridCell const& cell, bool displayColumn);

    private:
        double fillerLeftEdge_{};
        double desiredHeight_{};
        MiniDumpExplorer::DataGridRow owningRow_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridCellsPresenter : DataGridCellsPresenterT<DataGridCellsPresenter, implementation::DataGridCellsPresenter>
    {
    };
}
