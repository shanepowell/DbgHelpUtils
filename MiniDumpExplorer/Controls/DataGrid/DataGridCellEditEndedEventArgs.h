#pragma once

#include "DataGridCellEditEndedEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumn;
    struct DataGridRow;

    struct DataGridCellEditEndedEventArgs : DataGridCellEditEndedEventArgsT<DataGridCellEditEndedEventArgs>
    {
        DataGridCellEditEndedEventArgs(MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row, DataGridEditAction editAction);

        MiniDumpExplorer::DataGridColumn Column() const { return column_; }
        MiniDumpExplorer::DataGridRow Row() const { return row_; }
        DataGridEditAction EditAction() const { return editAction_; }

    private:
        MiniDumpExplorer::DataGridColumn column_;
        MiniDumpExplorer::DataGridRow row_;
        DataGridEditAction editAction_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridCellEditEndedEventArgs : DataGridCellEditEndedEventArgsT<DataGridCellEditEndedEventArgs, implementation::DataGridCellEditEndedEventArgs>
    {
    };
}
