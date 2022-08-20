#include "dump_mini_dump_heap.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/cache_manager.h"
#include "DbgHelpUtils/crt_entry.h"
#include "DbgHelpUtils/crt_heap.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/process_environment_block.h"
#include "DbgHelpUtils/stream_hex_dump.h"

using namespace std;
using namespace dlg_help_utils;

namespace 
{
    struct diff_crt_data
    {
        diff_crt_data(mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine)
        : peb{mini_dump, cache, symbol_engine}
        , crtheap{cache, peb}
        {
        }

        cache_manager cache;
        process::process_environment_block const peb;
        heap::crt_heap crtheap;
    };

    std::unique_ptr<diff_crt_data> setup_base_diff_dump_crtheap(std::unique_ptr<mini_dump> const& base_diff_dump, heap::crt_heap& crtheap, dbg_help::symbol_engine& symbol_engine)
    {
        if(!base_diff_dump)
        {
            return {};
        }

        auto base_diff_crtheap = std::make_unique<diff_crt_data>(*base_diff_dump, symbol_engine);
        crtheap.set_base_diff_filter(base_diff_crtheap->crtheap);
        return base_diff_crtheap;
    }
}

void dump_mini_dump_crtheap(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, cache, symbol_engine};
    auto const hex_length = peb.machine_hex_printable_length();

    heap::crt_heap heap{cache, peb};

    if(!heap.is_using_crt_heap())
    {
        log << L"Not using CRT Heap.\n";
        return;
    }

    [[maybe_unused]] auto const base_diff_crtheap = setup_base_diff_dump_crtheap(base_diff_dump, heap, symbol_engine);

    log << L"CRT Heap Entries:\n";
    std::wstring const indent_str(2, L' ');
    for (auto const& entry : heap.entries())
    {
        using namespace size_units::base_16;
        if(options.debug_heap_data())
        {
            log << std::format(L"{0}CRT Entry: {1}\n", indent_str, stream_hex_dump::to_hex(entry.entry_address(), hex_length));
            log << std::format(L"{0}  Block In Use: {1}\n", indent_str, entry.block_use());
            log << std::format(L"{0}  Filename: {1}\n", indent_str, entry.filename());
            log << std::format(L"{0}  Line Number: {1}\n", indent_str, locale_formatting::to_wstring(entry.line_number()));
            log << std::format(L"{0}  Request Number: {1}\n", indent_str, locale_formatting::to_wstring(entry.request_number()));
            log << std::format(L"{0}  Data Size: {1} ({2})\n", indent_str, to_wstring(entry.data_size()), stream_hex_dump::to_hex(entry.data_size()));
            log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
        }
        else
        {
            log << std::format(L"{0}CRT Entry: {1} {2} {3} UserPtr({4}) #{5}", indent_str
                , stream_hex_dump::to_hex(entry.entry_address(), hex_length)
                , entry.block_use() ? L"in use"sv : L"free"sv
                , to_wstring(entry.data_size())
                , stream_hex_dump::to_hex(entry.user_address(), hex_length)
                , locale_formatting::to_wstring(entry.request_number()));
            if(!entry.filename().empty())
            {
                log << std::format(L" : {0}:{1}", entry.filename(), locale_formatting::to_wstring(entry.line_number()));
            }
            log << L'\n';
        }

        if(entry.block_use() && options.hex_dump_memory_data())
        {
            uint64_t const size = entry.data_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), 4);
                log << L'\n';
            }
        }
    }
}

