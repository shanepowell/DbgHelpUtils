#pragma once

#include "DbgHelpUtils/memory_list_stream.h"
#include "DbgHelpUtils/memory64_list_stream.h"
#include "DbgHelpUtils/function_table_stream.h"
#include "DbgHelpUtils/module_list_stream.h"
#include "DbgHelpUtils/unloaded_module_list_stream.h"
#include "DbgHelpUtils/pe_file_memory_mapping.h"
#include "DbgHelpUtils/mini_dump_memory_walker.h"
#include "DbgHelpUtils/stream_thread.h"

namespace dlg_help_utils::dbg_help
{
    class symbol_engine;
}

namespace Utility
{
    struct mini_dump_walker_store
    {
        mini_dump_walker_store(dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::stream_thread const& thread, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);

        dlg_help_utils::memory_list_stream memory_list_;
        dlg_help_utils::memory64_list_stream memory64_list_;
        dlg_help_utils::function_table_stream function_table_;
        dlg_help_utils::module_list_stream module_list_;
        dlg_help_utils::unloaded_module_list_stream unloaded_module_list_;
        dlg_help_utils::pe_file_memory_mapping pe_file_memory_mappings_{};
        dlg_help_utils::stream_stack_dump::mini_dump_memory_walker walker_;
    };
}