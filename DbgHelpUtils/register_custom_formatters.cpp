#include "register_custom_formatters.h"

#include "gflags_custom_formatter.h"

void dlg_help_utils::ntdll_utilities::register_custom_formatters(symbol_type_utils::symbol_data_dumper& dumper)
{
    dumper.register_custom_type_formatter(std::make_unique<gflags_custom_formatter>());
}
