#pragma once
#include <memory>
#include <ostream>

class dump_file_options;

namespace dlg_help_utils
{
    namespace dbg_help
    {
        class symbol_engine;
    }

    class mini_dump;
}

void dump_mini_dump_heap(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_heap_entries(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_crtheap(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_heap_statistics(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
