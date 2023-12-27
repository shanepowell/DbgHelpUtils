#pragma once

#include "DataGridRowsPresenter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;

    struct DataGridRowsPresenter : DataGridRowsPresenterT<DataGridRowsPresenter>
    {
        DataGridRowsPresenter();

        MiniDumpExplorer::DataGrid const& OwningGrid() const { return owningGrid_; }
        void OwningGrid(MiniDumpExplorer::DataGrid const& value) { owningGrid_ = value; }

        // ReSharper disable CppHidingFunction
        Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size const& finalSize) const; // override
        Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const& availableSize) const; // override

        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const; // override
        // ReSharper restore CppHidingFunction

    private:
        void DataGridRowsPresenter_ManipulationStarting(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::ManipulationStartingRoutedEventArgs const& e);
        static void DataGridRowsPresenter_ManipulationStarted(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::ManipulationStartedRoutedEventArgs const& e);
        void DataGridRowsPresenter_ManipulationDelta(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs const& e) const;

        // ReSharper disable once CppFunctionIsNotImplemented
        auto InternalOwningGrid() const;

    private:
        MiniDumpExplorer::DataGrid owningGrid_{nullptr};
        double preManipulationHorizontalOffset_{};
        double preManipulationVerticalOffset_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowsPresenter : DataGridRowsPresenterT<DataGridRowsPresenter, implementation::DataGridRowsPresenter>
    {
    };
}
