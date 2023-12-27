#pragma once

#include "DataGridColumnEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumn;

    struct DataGridColumnEventArgs : DataGridColumnEventArgsT<DataGridColumnEventArgs>
    {
        DataGridColumnEventArgs(MiniDumpExplorer::DataGridColumn const& column);

        MiniDumpExplorer::DataGridColumn Column() const { return column_; }

    private:
        MiniDumpExplorer::DataGridColumn column_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridColumnEventArgs : DataGridColumnEventArgsT<DataGridColumnEventArgs, implementation::DataGridColumnEventArgs>
    {
    };
}
