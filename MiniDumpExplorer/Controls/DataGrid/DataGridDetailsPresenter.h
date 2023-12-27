#pragma once

#include "DataGridDetailsPresenter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridRow;
    struct DataGridCell;
    struct DataGridColumn;

    struct DataGridDetailsPresenter : DataGridDetailsPresenterT<DataGridDetailsPresenter>
    {
        DataGridDetailsPresenter();

        static Microsoft::UI::Xaml::DependencyProperty ContentHeightProperty();

        double ContentHeight() const;
        void ContentHeight(double value) const;

        MiniDumpExplorer::DataGridRow OwningRow() const { return owningRow_; }
        void OwningRow(MiniDumpExplorer::DataGridRow value) { owningRow_ = std::move(value); }  // NOLINT(performance-unnecessary-value-param)
        void EnsureFillerVisibility() const;
        void Recycle();

        Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size const& finalSize); // override
        Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const& availableSize); // override

    private:
        void OnContentHeightPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;

        MiniDumpExplorer::DataGrid OwningGrid() const;
        // ReSharper disable CppFunctionIsNotImplemented
        auto InternalOwningGrid() const;
        auto InternalOwningRow() const;
        // ReSharper restore CppFunctionIsNotImplemented

        bool ShouldDisplayCell(MiniDumpExplorer::DataGridColumn const& column, double frozenLeftEdge, double scrollingLeftEdge) const;

        static void EnsureCellClip(MiniDumpExplorer::DataGridCell const& cell, double width, double height, double frozenLeftEdge, double cellLeftEdge);
        static void EnsureCellDisplay(MiniDumpExplorer::DataGridCell const& cell, bool displayColumn);

    private:
        MiniDumpExplorer::DataGridRow owningRow_;
        double fillerLeftEdge_{0.0};
        double desiredHeight_{0.0};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridDetailsPresenter : DataGridDetailsPresenterT<DataGridDetailsPresenter, implementation::DataGridDetailsPresenter>
    {
    };
}
