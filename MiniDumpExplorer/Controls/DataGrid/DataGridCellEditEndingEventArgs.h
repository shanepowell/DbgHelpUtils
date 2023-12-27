#pragma once

#include "DataGridCellEditEndingEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumn;
    struct DataGridRow;

    struct DataGridCellEditEndingEventArgs : DataGridCellEditEndingEventArgsT<DataGridCellEditEndingEventArgs>
    {
        DataGridCellEditEndingEventArgs(MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row, Microsoft::UI::Xaml::FrameworkElement const& editingElement, DataGridEditAction editAction);

        MiniDumpExplorer::DataGridColumn Column() const { return column_; }
        MiniDumpExplorer::DataGridRow Row() const { return row_; }
        Microsoft::UI::Xaml::FrameworkElement EditingElement() const { return editingElement_; }
        DataGridEditAction EditAction() const { return editAction_; }
        bool Cancel() const { return cancelled_; }
        void Cancel(bool const value) { cancelled_ = value; }

    private:
        MiniDumpExplorer::DataGridColumn column_;
        MiniDumpExplorer::DataGridRow row_;
        Microsoft::UI::Xaml::FrameworkElement editingElement_;
        DataGridEditAction editAction_;
        std::atomic_bool cancelled_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridCellEditEndingEventArgs : DataGridCellEditEndingEventArgsT<DataGridCellEditEndingEventArgs, implementation::DataGridCellEditEndingEventArgs>
    {
    };
}
