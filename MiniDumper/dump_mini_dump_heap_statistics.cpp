#include "dump_mini_dump_heap.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/cache_manager.h"
#include "DbgHelpUtils/crt_heap.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/mini_dump_memory_walker.h"
#include "DbgHelpUtils/process_heaps.h"
#include "DbgHelpUtils/process_heaps_statistics.h"
#include "DbgHelpUtils/process_heaps_statistic_bucket_view.h"
#include "DbgHelpUtils/process_heaps_statistic_view.h"
#include "DbgHelpUtils/process_heap_entry.h"
#include "DbgHelpUtils/segment_heap.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_stack_dump.h"

using namespace std;
using namespace dlg_help_utils;

void dump_mini_dump_heap_statistics_view(std::wostream& log, process::process_environment_block const& peb, heap::process_heaps_statistic_view const& view_by_size_frequency, dump_file_options const& options, stream_stack_dump::is_x86_target_t const is_x86_target, streamsize const hex_length)
{
    using namespace size_units::base_16;
    log << std::format(L"  {}:\n", heap::process_heaps_statistic_view::to_wstring(view_by_size_frequency.view()));
    auto const single_line_range_title_length = 2 + hex_length;

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L" {0:<12} ", L"");
        log << std::format(L"{0:<{1}} ", L"", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:<12} {1:<12} ", L"range", L"range");
    }

    log << std::format(L"{:<12} ", L"allocated");
    log << std::format(L"{:<12} ", L"allocated");
    log << std::format(L"{:<12} ", L"allocated");
    log << std::format(L"{:<12} ", L"overhead");
    log << std::format(L"{:<12} ", L"free");
    log << std::format(L"{:<12} ", L"free");
    log << std::format(L"{:<7} ", L"count");
    log << std::format(L"{:<7} ", L"size");
    log << L"\n";

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L" {0:<12} ", L"size");
        log << std::format(L"{0:<{1}} ", L"size hex", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:<12} {1:<12} ", L"start", L"size");
    }

    log << std::format(L"{:<12} ", L"count");
    log << std::format(L"{:<12} ", L"total");
    log << std::format(L"{:<12} ", L"average");
    log << std::format(L"{:<12} ", L"total");
    log << std::format(L"{:<12} ", L"count");
    log << std::format(L"{:<12} ", L"total");
    log << std::format(L"{:<7} ", L"percent");
    log << std::format(L"{:<7} ", L"percent");
    log << L" application call site\n";

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L"={0:=<12}=", L"");
        log << std::format(L"{0:=<{1}}=", L"", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:=<12}={1:=<12}=", L"", L"");
    }

    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<7}=", L"");
    log << std::format(L"{:=<7}=", L"");
    log << L"======================\n";

    for(auto const& bucket : view_by_size_frequency.buckets())
    {
        if(view_by_size_frequency.is_range_single_value())
        {
            log << std::format(L" {:12} ", to_wstring(bucket.start_range()));
            log << std::format(L"{0:{1}} ", stream_hex_dump::to_hex(bucket.start_range()), single_line_range_title_length);
        }
        else
        {
            log << std::format(L" {0:12} {1:12} ", to_wstring(bucket.start_range()), to_wstring(bucket.end_range() - bucket.start_range()));
        }
        log << std::format(L"{:12} ", locale_formatting::to_wstring(bucket.allocated_count()));
        log << std::format(L"{:12} ", to_wstring(bucket.allocated_total()));
        log << std::format(L"{:12} ", to_wstring(bucket.allocated_average()));
        log << std::format(L"{:12} ", to_wstring(bucket.overhead_total()));
        log << std::format(L"{:12} ", locale_formatting::to_wstring(bucket.free_count()));
        log << std::format(L"{:12} ", to_wstring(bucket.free_total()));
        log << std::format(L"{:<7.2f} ", bucket.range_count_percent());
        log << std::format(L"{:<7.2f} ", bucket.range_size_percent());
        if(auto const& common_allocation_callsite = bucket.common_allocation_callsite();
            common_allocation_callsite.has_value())
        {
            log << std::format(L" {0}", hex_dump_stack_frame(common_allocation_callsite.value(), is_x86_target));
        }
        log << L'\n';

        if(options.display_symbols() && !bucket.allocation_stack_trace().empty())
        {
            log << L"  Allocation Stack Trace:\n";
            hex_dump_stack(log, peb.walker(), bucket.allocation_stack_trace(), is_x86_target, 2);
            log << L'\n';
        }

        if(options.debug_heap_data())
        {
            log << L"  Entries:\n";
            for(auto const& entry : bucket.entries())
            {
                detail::print_process_entry(log, entry, peb, hex_length, options, 4);
            }

            log << L"  Free Entries:\n";
            for(auto const& entry : bucket.free_entries())
            {
                detail::print_process_entry(log, entry, peb, hex_length, options, 4);
            }
        }
    }

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L"={0:=<12}=", L"");
        log << std::format(L"{0:=<{1}}=", L"", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:=<12}={1:=<12}=", L"", L"");
    }

    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<8}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<7}=", L"");
    log << std::format(L"{:=<7}=", L"");
    log << L"======================\n";

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L" {:12} ", L"");
        log << std::format(L"{0:>{1}} ", L"Total:", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:12} {1:>12} ", L"", L"Total:");
    }
    log << std::format(L"{:12} ", locale_formatting::to_wstring(view_by_size_frequency.allocated_count()));
    log << std::format(L"{:12} ", to_wstring(view_by_size_frequency.allocated_total()));
    log << std::format(L"{:12} ", to_wstring(view_by_size_frequency.allocated_average()));
    log << std::format(L"{:12} ", to_wstring(view_by_size_frequency.overhead_total()));
    log << std::format(L"{:12} ", locale_formatting::to_wstring(view_by_size_frequency.free_count()));
    log << std::format(L"{:12} ", to_wstring(view_by_size_frequency.free_total()));
    log << L'\n';
}

void dump_mini_dump_heap_statistics(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    heap::process_heaps heaps{mini_dump, cache, symbol_engine, options.process_heaps_options(), options.system_module_list(), options.statistic_view_options()};
    cache_manager base_cache;
    [[maybe_unused]] auto const base_diff_heaps = detail::setup_base_diff_dump_heaps(base_diff_dump, base_cache, heaps, symbol_engine, options);

    auto const loading_heap_statistics = L"Loading heap statistics..."s;
    std::wstring const move_back(loading_heap_statistics.size(), L'\b');
    std::wstring const clear(loading_heap_statistics.size(), L' ');
    std::wcerr << loading_heap_statistics;

    auto const hex_length = heaps.peb().machine_hex_printable_length();
    auto const is_x86_target = stream_stack_dump::is_x86_target_t{heaps.peb().is_x86_target()};
    auto const statistics = heaps.statistics();

    std::wcerr << move_back;
    std::wcerr << clear;
    std::wcerr << move_back;

    log << L"Heap Statistics:\n";
    if(options.display_heap_statistic_view(heap_statistics_view::by_size_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(log, heaps.peb(), statistics.view_by_size_frequency(), options, is_x86_target, hex_length);
        log << L'\n';
    }
    if(options.display_heap_statistic_view(heap_statistics_view::by_size_ranges_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(log, heaps.peb(), statistics.view_by_size_ranges_frequency(), options, is_x86_target, hex_length);
        log << L'\n';
    }
    if(options.display_heap_statistic_view(heap_statistics_view::by_stacktrace_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(log, heaps.peb(), statistics.view_by_stacktrace_frequency(), options, is_x86_target, hex_length);
        log << L'\n';
    }
    if(options.display_heap_statistic_view(heap_statistics_view::by_application_callsite_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(log, heaps.peb(), statistics.view_by_application_callsite_frequency(), options, is_x86_target, hex_length);
        log << L'\n';
    }
}

