#pragma once
#include <memory>
#include <ostream>

#include "DbgHelpUtils/tagged_bool.h"

class dump_file_options;

namespace dlg_help_utils
{
    namespace process
    {
        class process_environment_block;
    }

    namespace heap
    {
        class nt_heap;
        class segment_heap;
        class dph_heap;
        class process_heaps;
        class process_heap_entry;
    }

    class cache_manager;

    namespace dbg_help
    {
        class symbol_engine;
    }

    class mini_dump;
}

void dump_mini_dump_heap(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::cache_manager& cache, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_heap_entries(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::cache_manager& cache, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_crtheap(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::cache_manager& cache, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_heap_statistics(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::cache_manager& cache, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
void dump_mini_dump_heap_graph(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::cache_manager& cache, dump_file_options const& options, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);

namespace detail
{
    constexpr auto units_max_width = 16;

    std::unique_ptr<dlg_help_utils::heap::process_heaps> setup_base_diff_dump_heaps(std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dlg_help_utils::cache_manager& base_cache, dlg_help_utils::heap::process_heaps& heaps, dlg_help_utils::dbg_help::symbol_engine& symbol_engine, dump_file_options const& options);
    void print_process_entry(std::wostream& log, dlg_help_utils::heap::process_heap_entry const& entry, dlg_help_utils::process::process_environment_block const& peb, std::streamsize hex_length, dump_file_options const& options, size_t indent);

    using is_process_heap_t = dlg_help_utils::tagged_bool<struct is_process_heap_type>;
    std::wstring_view get_process_marker(is_process_heap_t is_process_heap);

    void print_nt_heap_line(std::wostream& log, std::wstring_view const& process_heap_marker, dlg_help_utils::heap::nt_heap const& nt_heap);
    void print_nt_heap(std::wostream& log, std::streamsize hex_length, dump_file_options const& options, dlg_help_utils::heap::nt_heap const& nt_heap, dlg_help_utils::process::process_environment_block const& peb);
    void print_nt_heap_segments_list(std::wostream& log, std::streamsize hex_length, dlg_help_utils::heap::nt_heap const& nt_heap, size_t indent);
    void print_nt_heap_lfh_segments_list(std::wostream& log, std::streamsize hex_length, dlg_help_utils::heap::nt_heap const& nt_heap, size_t indent);

    void print_segment_heap(std::wostream& log, std::streamsize hex_length, std::wstring_view const& process_heap_marker, dlg_help_utils::heap::segment_heap const& segment_heap, dump_file_options const& options, size_t indent);
    void print_segment_heap_line(std::wostream& log, std::wstring_view const& process_heap_marker, dlg_help_utils::heap::segment_heap const& segment_heap);

    void print_debug_page_heap(std::wostream& log, std::streamsize hex_length, dlg_help_utils::heap::dph_heap const& heap, dump_file_options const& options, size_t indent);
}

