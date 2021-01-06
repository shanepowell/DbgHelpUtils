#pragma once
#include "../DbgHelpUtils/function_table_stream.h"
#include "../DbgHelpUtils/memory64_list_stream.h"
#include "../DbgHelpUtils/memory_list_stream.h"
#include "../DbgHelpUtils/module_list_stream.h"
#include "../DbgHelpUtils/pe_file_memory_mapping.h"
#include "../DbgHelpUtils/unloaded_module_list_stream.h"

struct MiniDumpData
{
    MiniDumpData(dlg_help_utils::mini_dump const& mini_dump);

    dlg_help_utils::memory_list_stream const memory_list_;
    dlg_help_utils::memory64_list_stream const memory64_list_;
    dlg_help_utils::function_table_stream const function_table_;
    dlg_help_utils::module_list_stream const module_list_;
    dlg_help_utils::unloaded_module_list_stream const unloaded_module_list_;
    dlg_help_utils::pe_file_memory_mapping pe_file_memory_mappings_;
};
