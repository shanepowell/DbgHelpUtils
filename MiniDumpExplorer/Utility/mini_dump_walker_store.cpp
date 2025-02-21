#include "pch.h"
#include "mini_dump_walker_store.h"

namespace Utility
{
    mini_dump_walker_store::mini_dump_walker_store(dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::stream_thread const& thread, dlg_help_utils::dbg_help::symbol_engine& symbol_engine)
        : memory_list_{ mini_dump }
        , memory64_list_{ mini_dump }
        , function_table_{ mini_dump }
        , module_list_{ mini_dump }
        , unloaded_module_list_{ mini_dump }
        , walker_
        {
            thread->Stack.StartOfMemoryRange,
            thread.stack(),
            thread->Stack.Memory.DataSize,
            memory_list_,
            memory64_list_,
            function_table_,
            module_list_,
            unloaded_module_list_,
            pe_file_memory_mappings_,
            symbol_engine
        }
    {
    }
}