#pragma once

#include "DataGridRowEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRow;

    struct DataGridRowEventArgs : DataGridRowEventArgsT<DataGridRowEventArgs>
    {
        DataGridRowEventArgs(MiniDumpExplorer::DataGridRow const& dataGridRow);

        MiniDumpExplorer::DataGridRow Row() const { return row_; }

    private:
        MiniDumpExplorer::DataGridRow row_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowEventArgs : DataGridRowEventArgsT<DataGridRowEventArgs, implementation::DataGridRowEventArgs>
    {
    };
}
