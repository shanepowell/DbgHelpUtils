#pragma once
#include "symbol_data_dumper.h"

namespace dlg_help_utils
{
    class cache_manager;
}

namespace dlg_help_utils::ntdll_utilities
{
    void register_custom_formatters(symbol_type_utils::symbol_data_dumper& dumper, cache_manager& cache);
};
