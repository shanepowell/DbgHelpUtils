#pragma once

#include "DataGridCellEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridCell;

    struct DataGridCellEventArgs : DataGridCellEventArgsT<DataGridCellEventArgs>
    {
        DataGridCellEventArgs(MiniDumpExplorer::DataGridCell const& dataGridCell);

        MiniDumpExplorer::DataGridCell Cell() const { return cell_; }

    private:
        MiniDumpExplorer::DataGridCell cell_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridCellEventArgs : DataGridCellEventArgsT<DataGridCellEventArgs, implementation::DataGridCellEventArgs>
    {
    };
}
