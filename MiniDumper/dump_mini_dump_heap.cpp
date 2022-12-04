#include "dump_mini_dump_heap.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/cache_manager.h"
#include "DbgHelpUtils/crt_entry.h"
#include "DbgHelpUtils/crt_heap.h"
#include "DbgHelpUtils/dph_heap.h"
#include "DbgHelpUtils/heap_segment.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/memory_range.h"
#include "DbgHelpUtils/mini_dump_memory_walker.h"
#include "DbgHelpUtils/nt_heap.h"
#include "DbgHelpUtils/process_heaps.h"
#include "DbgHelpUtils/process_heap_entry.h"
#include "DbgHelpUtils/segment_heap.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_stack_dump.h"

using namespace std;
using namespace dlg_help_utils;

void dump_mini_dump_heap(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, cache, symbol_engine};
    auto const hex_length = peb.machine_hex_printable_length();

    log << L"Heaps:\n";
    log << std::format(L"{1:<{0}} NT/Segment   Flags      {3:<{2}} {4:<{2}} {5:<{2}} {6:<{2}} {7:<{2}} {8:<8} {9:<4}\n"
        , hex_length + 3, L"Address"sv
        , detail::units_max_width
        , L"Reserved"sv
        , L"Committed"sv
        , L"Uncommitted"sv
        , L"Virtual"sv
        , L"Free"sv
        , L"Segments"sv
        , L"UCRs"sv);
    for(uint32_t heap_index = 0; heap_index < peb.number_of_heaps(); ++heap_index)
    {
        auto const heap_address = peb.heap_address(heap_index);
        log << ' ' << stream_hex_dump::to_hex(heap_address, hex_length);

        if(auto const segment_signature = peb.segment_signature(heap_index); segment_signature == heap::SegmentSignatureNtHeap)
        {
            if(auto const nt_heap = peb.nt_heap(heap_index); nt_heap.has_value())
            {
                detail::print_nt_heap_line(log, get_process_marker(detail::is_process_heap_t{nt_heap.value().is_process_heap(peb.process_heap())}), nt_heap.value());
                detail::print_nt_heap_segments_list(log, hex_length, nt_heap.value(), 2);
                detail::print_nt_heap_lfh_segments_list(log, hex_length, nt_heap.value(), 2);
            }
        }
        else if(segment_signature == heap::SegmentSignatureSegmentHeap)
        {
            if(auto const segment_heap = peb.segment_heap(heap_index, options.process_heaps_options().segment_heap_options()); segment_heap.has_value())
            {
                detail::print_segment_heap_line(log, get_process_marker(detail::is_process_heap_t{peb.process_heap() == heap_address}), segment_heap.value());
            }
        }
        else
        {
            log << std::format(L" Unknown Heap Type ({0})\n", stream_hex_dump::to_hex(segment_signature));
        }
    }

    log << L'\n';

    for(uint32_t heap_index = 0; heap_index < peb.number_of_heaps(); ++heap_index)
    {
        if(auto const nt_heap = peb.nt_heap(heap_index); nt_heap.has_value())
        {
            detail::print_nt_heap(log, hex_length, options, nt_heap.value(), peb);
        }
        else if(auto const segment_heap = peb.segment_heap(heap_index, options.process_heaps_options().segment_heap_options()); segment_heap.has_value())
        {
            detail::print_segment_heap(log, hex_length, get_process_marker(detail::is_process_heap_t{peb.process_heap() == segment_heap.value().segment_heap_address()}), segment_heap.value(), options, 0);
        }
    }

    for(auto const& heap : heap::dph_heap::dph_heaps(cache, peb))
    {
        detail::print_debug_page_heap(log, hex_length, heap, options, 2);
    }

    log << L"\nVirtual Allocation Memory Ranges:\n";
    log << std::format(L" {1:<{0}} {2:<{0}} {3:<{0}}\n", hex_length + 2, L"Start"sv, L"End"sv, L"Size"sv);
    using namespace size_units::base_16;
    for(auto const& range : peb.walker().memory_ranges())
    {
        log << std::format(L" {0} {1} {2} ({3})\n", stream_hex_dump::to_hex(range.start_range, hex_length), stream_hex_dump::to_hex(range.end_range, hex_length), stream_hex_dump::to_hex(range.end_range - range.start_range, hex_length), to_wstring(bytes{range.end_range - range.start_range}));
    }

    log << L'\n';
}

void dump_mini_dump_heap_entries(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    heap::process_heaps heaps{mini_dump, cache, symbol_engine, options.process_heaps_options(), options.system_module_list(), options.statistic_view_options()};
    cache_manager base_cache;
    [[maybe_unused]] auto const base_diff_heaps = detail::setup_base_diff_dump_heaps(base_diff_dump, base_cache, heaps, symbol_engine, options);
    auto const hex_length = heaps.peb().machine_hex_printable_length();

    log << L"Heap Allocated Entries:\n";
    for(auto const& entry : heaps.entries())
    {
        detail::print_process_entry(log, entry, heaps.peb(), hex_length, options, 2);
    }

    log << L"Heap Free Entries:\n";
    for(auto const& entry : heaps.free_entries())
    {
        detail::print_process_entry(log, entry, heaps.peb(), hex_length, options, 2);
    }

    log << L'\n';
}

