#include <format>

#include "dump_file_options.h"
#include "dump_mini_dump_heap.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/process_heaps.h"
#include "DbgHelpUtils/process_heap_entry.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_stack_dump.h"

using namespace std;
using namespace dlg_help_utils;

namespace detail
{
    std::unique_ptr<heap::process_heaps> setup_base_diff_dump_heaps(std::unique_ptr<mini_dump> const& base_diff_dump, cache_manager& base_cache, heap::process_heaps& heaps, dbg_help::symbol_engine& symbol_engine, dump_file_options const& options)
    {
        if(!base_diff_dump)
        {
            return {};
        }

        auto base_diff_heaps = std::make_unique<heap::process_heaps>(*base_diff_dump, base_cache, symbol_engine, options.process_heaps_options(), options.system_module_list(), options.statistic_view_options());
        heaps.set_base_diff_filter(*base_diff_heaps);
        return base_diff_heaps;
    }

    void print_process_entry(std::wostream& log, heap::process_heap_entry const& entry, process::process_environment_block const& peb, std::streamsize const hex_length, dump_file_options const& options, size_t const indent)
    {
        using namespace size_units::base_16;
        log << std::format(L"{0:{1}}{2} size({3}) overhead({4})", L' ', indent, stream_hex_dump::to_hex(entry.user_address(), hex_length), to_wstring(entry.user_requested_size()), to_wstring(entry.overhead_size()));
        if(!entry.filename().empty())
        {
            log << std::format(L" {0}:{1}", entry.filename(), locale_formatting::to_wstring(entry.line_number()));
        }
        if(entry.has_request_number())
        {
            log << std::format(L" #{}", locale_formatting::to_wstring(entry.request_number()));
        }
        if(!entry.allocation_stack_trace().empty())
        {
            log << L" (has stack)";
        }
        log << L'\n';

        if(options.display_symbols() && !entry.allocation_stack_trace().empty())
        {
            log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
            hex_dump_stack(log, peb.walker(), entry.allocation_stack_trace(), peb.is_x86_target(), indent + 4);
            log << L'\n';
        }

        if(options.hex_dump_memory_data())
        {
            if(auto stream = entry.all_user_data(); !stream.eof())
            {
                hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(entry.user_requested_size().count()), indent + 4);
                log << L'\n';
            }
        }
    }

    std::wstring_view get_process_marker(bool const is_process_heap)
    {
        return is_process_heap ? L" (process heap)"sv : L""sv;
    }
}
