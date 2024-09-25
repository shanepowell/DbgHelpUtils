#pragma once

namespace DataGridInternal
{
    enum class DataGridSelectionAction : uint8_t
    {
        AddCurrentToSelection,
        None,
        RemoveCurrentFromSelection,
        SelectCurrent,
        SelectFromAnchorToCurrent
    };
}
