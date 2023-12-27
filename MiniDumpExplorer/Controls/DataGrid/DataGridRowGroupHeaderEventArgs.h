#pragma once

#include "DataGridRowGroupHeaderEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRowGroupHeader;

    struct DataGridRowGroupHeaderEventArgs : DataGridRowGroupHeaderEventArgsT<DataGridRowGroupHeaderEventArgs>
    {
        DataGridRowGroupHeaderEventArgs(MiniDumpExplorer::DataGridRowGroupHeader const& rowGroupHeader);

        MiniDumpExplorer::DataGridRowGroupHeader RowGroupHeader() const { return rowGroupHeader_; }

    private:
        MiniDumpExplorer::DataGridRowGroupHeader rowGroupHeader_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowGroupHeaderEventArgs : DataGridRowGroupHeaderEventArgsT<DataGridRowGroupHeaderEventArgs, implementation::DataGridRowGroupHeaderEventArgs>
    {
    };
}
