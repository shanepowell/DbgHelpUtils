#pragma once

namespace DataGridInternal
{
    struct DataGridCellCoordinates
    {
        int32_t ColumnIndex;
        int32_t Slot;

        auto operator<=>(DataGridCellCoordinates const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)
    };
}
