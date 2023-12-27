#pragma once

#include "DataGridRowEditEndingEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRow;

    struct DataGridRowEditEndingEventArgs : DataGridRowEditEndingEventArgsT<DataGridRowEditEndingEventArgs>
    {
        DataGridRowEditEndingEventArgs(MiniDumpExplorer::DataGridRow const& row, DataGridEditAction editAction);

        MiniDumpExplorer::DataGridRow Row() const { return row_; }
        DataGridEditAction EditAction() const { return editAction_; }
        bool Cancel() const { return cancelled_; }
        void Cancel(bool const value) { cancelled_ = value; }

    private:
        MiniDumpExplorer::DataGridRow row_;
        DataGridEditAction editAction_;
        std::atomic_bool cancelled_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowEditEndingEventArgs : DataGridRowEditEndingEventArgsT<DataGridRowEditEndingEventArgs, implementation::DataGridRowEditEndingEventArgs>
    {
    };
}
