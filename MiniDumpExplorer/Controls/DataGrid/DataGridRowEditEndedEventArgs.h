#pragma once

#include "DataGridRowEditEndedEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRow;

    struct DataGridRowEditEndedEventArgs : DataGridRowEditEndedEventArgsT<DataGridRowEditEndedEventArgs>
    {
        DataGridRowEditEndedEventArgs(MiniDumpExplorer::DataGridRow const& row, DataGridEditAction editAction);

        MiniDumpExplorer::DataGridRow Row() const { return row_; }
        DataGridEditAction EditAction() const { return editAction_; }

    private:
        MiniDumpExplorer::DataGridRow row_;
        DataGridEditAction editAction_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowEditEndedEventArgs : DataGridRowEditEndedEventArgsT<DataGridRowEditEndedEventArgs, implementation::DataGridRowEditEndedEventArgs>
    {
    };
}
