#include "pch.h"
#include "MiniDumpData.h"

MiniDumpData::MiniDumpData(dlg_help_utils::mini_dump const& mini_dump)
    : memory_list_{mini_dump}
    , memory64_list_{mini_dump}
    , function_table_{mini_dump}
    , module_list_{mini_dump}
    , unloaded_module_list_{mini_dump}
{
}
