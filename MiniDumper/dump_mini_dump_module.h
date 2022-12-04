#pragma once

#include <ostream>

class dump_file_options;

namespace dlg_help_utils
{
    class cache_manager;

    namespace dbg_help
    {
        class symbol_engine;
    }

    class mini_dump;
}

void dump_mini_dump_module_list_stream_data(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, size_t index, dump_file_options const& options);
void dump_mini_dump_unloaded_module_list_stream_data(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, size_t index);

void dump_mini_dump_loaded_modules(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::cache_manager& cache, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
