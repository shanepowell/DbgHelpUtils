#include "dump_file_options.h"

#include "dump_mini_dump_heap.h"
#include "DbgHelpUtils/dph_entry.h"
#include "DbgHelpUtils/dph_heap.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/i_value_type_formatter.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/process_environment_block.h"
#include "DbgHelpUtils/stream_hex_dump.h"

using namespace std;
using namespace dlg_help_utils;

namespace detail
{
    namespace
    {
        void print_debug_page_heap_entry_single_line(
            std::wostream& log
            , i_value_type_formatter const& formatter
            , size_t const index
            , heap::dph_entry const& entry
            , size_t const indent)
        {
            using namespace size_units::base_16;
            log << std::format(L"{0:{1}}{2} @ {3} {4} BlockPtr({5}) BlockSize({6}) UserPtr({7}) ReqSize({8})\n", ' ', indent
                , locale_formatting::to_wstring(index)
                , formatter.format_pointer_value(entry.entry_address(), true)
                , entry.is_allocated() ? L"Busy"sv : L"Free"sv
                , formatter.format_pointer_value(entry.virtual_block_address(), true)
                , to_wstring(entry.virtual_block_size())
                , formatter.format_pointer_value(entry.user_address(), true)
                , to_wstring(entry.user_requested_size()));
        }

        void print_debug_page_heap_entry_debug(
            std::wostream& log
            , i_value_type_formatter const& formatter
            , size_t const index
            , heap::dph_entry const& entry
            , size_t const indent)
        {
            using namespace size_units::base_16;
            std::wstring const indent_str(indent, L' ');
            log << std::format(L"{0}{1} @ {2}\n", indent_str, locale_formatting::to_wstring(index), formatter.format_pointer_value(entry.entry_address(), entry.walker().is_memory_valid(entry.entry_address())));
            log << std::format(L"{0}  Is Allocated: {1}\n", indent_str, entry.is_allocated());
            log << std::format(L"{0}  Virtual Block: {1}\n", indent_str, formatter.format_pointer_value(entry.virtual_block_address(), entry.walker().is_memory_valid(entry.virtual_block_address())));
            log << std::format(L"{0}  Virtual Block Size: {1} ({2})\n", indent_str, to_wstring(entry.virtual_block_size()), stream_hex_dump::to_hex(entry.virtual_block_size().count()));
            log << std::format(L"{0}  User Allocation: {1}\n", indent_str, formatter.format_pointer_value(entry.user_address(), entry.walker().is_memory_valid(entry.user_address())));
            log << std::format(L"{0}  User Requested Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size().count()));
            log << std::format(L"{0}  UST Address: {1}\n", indent_str, formatter.format_pointer_value(entry.ust_address(), entry.walker().is_memory_valid(entry.ust_address())));
        }

        void print_debug_page_heap_entry(
            std::wostream& log
            , i_value_type_formatter const& formatter
            , size_t const index
            , heap::dph_entry const& entry
            , dump_file_options const& options
            , size_t const indent)
        {
            if(options.debug_heap_data())
            {
                print_debug_page_heap_entry_debug(log, formatter, index, entry, indent);
            }
            else
            {
                print_debug_page_heap_entry_single_line(log, formatter, index, entry, indent);
            }

            if(options.display_symbols() && entry.is_allocated() && !entry.allocation_stack_trace().empty())
            {
                std::wstring const indent_str(indent, L' ');
                log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
                dump_stack_to_stream(log, entry.heap().walker(), entry.allocation_stack_trace(), formatter, indent + 4);
                log << L'\n';
            }

            if(options.hex_dump_memory_data() && entry.is_allocated() && entry.user_address() != 0)
            {
                uint64_t const size = entry.user_requested_size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 4);
                    log << L'\n';
                }
            }
        }
    }

    void print_debug_page_heap(
        std::wostream& log
        , i_value_type_formatter const& formatter
        , heap::dph_heap const& heap
        , dump_file_options const& options
        , size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}Debug Page Heap: {1}\n", indent_str, formatter.format_pointer_value(heap.address(), heap.walker().is_memory_valid(heap.address())));
        log << std::format(L"{0}  Flags: {1}\n", indent_str, stream_hex_dump::to_hex(heap.flags()));
        log << std::format(L"{0}  Extra Flags: {1}\n", indent_str, stream_hex_dump::to_hex(heap.extra_flags()));
        log << std::format(L"{0}  Seed: {1}\n", indent_str, stream_hex_dump::to_hex(heap.seed()));
        log << std::format(L"{0}  Normal Heap: {1}\n", indent_str, formatter.format_pointer_value(heap.normal_heap(), heap.walker().is_memory_valid(heap.normal_heap())));
        log << std::format(L"{0}  Busy Allocations: {1}\n", indent_str, locale_formatting::to_wstring(heap.busy_allocations()));
        log << std::format(L"{0}  Busy Allocations Committed Total: {1} ({2})\n", indent_str, to_wstring(heap.busy_allocations_committed()), stream_hex_dump::to_hex(heap.busy_allocations_committed()));
        log << std::format(L"{0}  Virtual Ranges: {1}\n", indent_str, locale_formatting::to_wstring(heap.virtual_storage_ranges()));
        log << std::format(L"{0}  Virtual Ranges Total: {1} ({2})\n", indent_str, to_wstring(heap.virtual_storage_total()), stream_hex_dump::to_hex(heap.virtual_storage_total()));
        log << std::format(L"{0}  Free Allocations: {1}\n", indent_str, locale_formatting::to_wstring(heap.free_allocations()));
        log << std::format(L"{0}  Free Allocations Committed Total: {1} ({2})\n", indent_str, to_wstring(heap.free_allocations_committed()), stream_hex_dump::to_hex(heap.free_allocations_committed()));
        log << std::format(L"{0}  Busy Entries: ({1})\n", indent_str, locale_formatting::to_wstring(heap.busy_allocations()));

        size_t index = 1;
        for(auto const& entry : heap.busy_entries())
        {
            print_debug_page_heap_entry(log, formatter, index, entry, options, indent + 4);
            ++index;
        }

        log << std::format(L"{0}  Virtual Ranges: ({1})\n", indent_str, locale_formatting::to_wstring(heap.virtual_storage_ranges()));

        index = 1;
        for(auto const& entry : heap.virtual_ranges())
        {
            print_debug_page_heap_entry(log, formatter, index, entry, options, indent + 4);
            ++index;
        }

        log << std::format(L"{0}  Free Allocations: ({1})\n", indent_str, locale_formatting::to_wstring(heap.free_allocations()));

        index = 1;
        for(auto const& entry : heap.free_entries())
        {
            print_debug_page_heap_entry(log, formatter, index, entry, options, indent + 4);
            ++index;
        }
        log << L'\n';
    }
}
