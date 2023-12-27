#pragma once

#include "DataGrid.h"
#include "DataGridColumn.h"
#include "DataGridColumnHeader.h"

namespace winrt::MiniDumpExplorer::implementation
{
    inline auto DataGridColumn::InternalOwningGrid() const
    {
        return owningGrid_.try_as<DataGrid>();
    }

    inline auto DataGridColumn::InternalHeaderCell() const
    {
        return headerCell_.try_as<DataGridColumnHeader>();
    }
}