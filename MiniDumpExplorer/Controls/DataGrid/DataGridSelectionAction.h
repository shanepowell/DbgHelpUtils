#pragma once

namespace DataGridInternal
{
    enum class DataGridSelectionAction
    {
        AddCurrentToSelection,
        None,
        RemoveCurrentFromSelection,
        SelectCurrent,
        SelectFromAnchorToCurrent
    };
}
