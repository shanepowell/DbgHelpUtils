#pragma once

#include "DataGrid.h"
#include "DataGridCell.h"
#include "DataGridRow.h"

namespace winrt::MiniDumpExplorer::implementation
{
    inline auto DataGridCell::InternalOwningColumn() const
    {
        return owningColumn_.try_as<DataGridColumn>();
    }

    inline auto DataGridCell::InternalOwningRow() const
    {
        return owningRow_.try_as<DataGridRow>();
    }

    inline auto DataGridCell::InternalOwningGrid() const
    {
        return InternalOwningRow()->OwningGrid().try_as<DataGrid>();
    }
}