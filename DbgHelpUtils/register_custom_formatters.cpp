#include "register_custom_formatters.h"

#include "gflags_custom_formatter.h"
#include "list_entry_custom_formatter.h"
#include "single_list_entry_custom_formatter.h"
#include "string_custom_formatter.h"
#include "unicode_string_custom_formatter.h"

void dlg_help_utils::ntdll_utilities::register_custom_formatters(symbol_type_utils::symbol_data_dumper& dumper, cache_manager& cache)
{
    dumper.register_custom_type_formatter(std::make_unique<gflags_custom_formatter>());
    dumper.register_custom_type_formatter(std::make_unique<unicode_string_custom_formatter>(cache));
    dumper.register_custom_type_formatter(std::make_unique<string_custom_formatter>(cache));
    dumper.register_custom_type_formatter(std::make_unique<list_entry_custom_formatter>(cache));
    dumper.register_custom_type_formatter(std::make_unique<single_list_entry_custom_formatter>(cache));
}
