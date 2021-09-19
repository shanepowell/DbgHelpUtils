// ValidateHeapEntries.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#define NOMINMAX 1
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable : 4100 4458)
#include <lyra/lyra.hpp>
#pragma warning(pop)

#include "ResultSet.h"
#include "symbol_engine_ui.h"
#include "DbgHelpUtils/crt_entry.h"
#include "DbgHelpUtils/crt_heap.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/process_heaps.h"
#include "DbgHelpUtils/process_heap_entry.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/statistic_view_options.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/string_conversation.h"
#include "DbgHelpUtils/system_module_list.h"

namespace ConsoleForeground
{
    constexpr WORD BLACK = 0;
    constexpr WORD DARKBLUE = FOREGROUND_BLUE;
    constexpr WORD DARKGREEN = FOREGROUND_GREEN;
    constexpr WORD DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE;
    constexpr WORD DARKRED = FOREGROUND_RED;
    constexpr WORD DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE;
    constexpr WORD DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
    constexpr WORD DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    constexpr WORD GRAY = FOREGROUND_INTENSITY;
    constexpr WORD BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
    constexpr WORD GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
    constexpr WORD CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
    constexpr WORD RED = FOREGROUND_INTENSITY | FOREGROUND_RED;
    constexpr WORD MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
    constexpr WORD YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
    constexpr WORD WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
}


struct base_dump_file
{
    base_dump_file(std::wstring const& dump_filename, std::wostream* o_log, dlg_help_utils::dbg_help::symbol_engine& symbol_engine, dlg_help_utils::heap::statistic_views::system_module_list const& system_module_list, dlg_help_utils::heap::statistic_views::statistic_view_options const& statistic_view_options)
    : dump_file{open_mini_dump_file(dump_filename, o_log)}
    , heaps{dump_file, symbol_engine, system_module_list, statistic_view_options}
    , crt_heap{heaps.peb()}
    {
    }

    dlg_help_utils::mini_dump dump_file;
    dlg_help_utils::heap::process_heaps heaps;
    dlg_help_utils::heap::crt_heap crt_heap;

private:
    static dlg_help_utils::mini_dump open_mini_dump_file(std::wstring const& dump_filename, std::wostream* o_log)
    {
        dlg_help_utils::mini_dump dump_file{dump_filename};
        dump_file.open_mini_dump();
        if(dump_file.type() != dlg_help_utils::dump_file_type::user_mode_dump)
        {
            *o_log << "ERROR: invalid base dmp file [" << dump_filename << "]\n";
            throw std::runtime_error{"invalid base dmp file"};
        }

        return dump_file;
    }
};

std::unique_ptr<base_dump_file> make_base_heap(std::wstring const& dump_filename, std::wostream* o_log, dlg_help_utils::heap::process_heaps& heaps, dlg_help_utils::heap::crt_heap& crt_heap, dlg_help_utils::dbg_help::symbol_engine& symbol_engine, dlg_help_utils::heap::statistic_views::system_module_list const& system_module_list, dlg_help_utils::heap::statistic_views::statistic_view_options const& statistic_view_options)
{
    if(dump_filename.empty())
    {
        return {};
    }

    try
    {
        auto rv = std::make_unique<base_dump_file>(dump_filename, o_log, symbol_engine, system_module_list, statistic_view_options);
        heaps.set_base_diff_filter(rv->heaps);
        crt_heap.set_base_diff_filter(rv->crt_heap);
        return rv;
    }
    catch(std::runtime_error const&)
    {
        return {};
    }
}

bool validate_dump_file(bool stacktrace, std::wstring const& dump_filename, std::wstring const& base_dump_filename, std::wostream* o_log, std::vector<Allocation> const& allocations)
{
    dlg_help_utils::mini_dump dump_file{dump_filename};
    dump_file.open_mini_dump();
    if(dump_file.type() != dlg_help_utils::dump_file_type::user_mode_dump)
    {
        *o_log << "ERROR: invalid dmp file [" << dump_filename << "]\n";
        return false;
    }

    using namespace dlg_help_utils::size_units::base_16;
    symbol_engine_ui ui;
    dlg_help_utils::dbg_help::symbol_engine symbol_engine{ui};
    dlg_help_utils::heap::statistic_views::system_module_list system_module_list;
    dlg_help_utils::heap::statistic_views::statistic_view_options statistic_view_options;

    dlg_help_utils::heap::process_heaps heaps{dump_file, symbol_engine, system_module_list, statistic_view_options};
    dlg_help_utils::heap::crt_heap crt_heap{ heaps.peb() };

    // ReSharper disable once CppTooWideScopeInitStatement
    auto base_heap = make_base_heap(base_dump_filename, o_log, heaps, crt_heap, symbol_engine, system_module_list, statistic_view_options);

    if(!base_dump_filename.empty() && !base_heap)
    {
        return true;
    }

    *o_log << "Processing dmp [" << dump_filename << "]\n";

    bool successful = true;

    std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> heap_allocations;
    auto const hex_length = heaps.peb().machine_hex_printable_length();
    for(auto const& entry : heaps.entries())
    {
        if(entry.user_address() == 0)
        {
            *o_log << "ERROR: CRT allocation == nullptr\n";
            successful = false;
        }
        else
        {
            heap_allocations.insert(std::make_pair(entry.user_address(), entry));
        }
    }

    std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> heap_free_entries;
    for(auto const& entry : heaps.free_entries())
    {
        heap_free_entries.insert(std::make_pair(entry.user_address(), entry));
    }

    for(auto const& allocation : allocations)
    {
        if(heap_allocations.empty())
        {
            if(allocation.allocated)
            {
                *o_log << "ERROR: can't find allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "]\n";
                successful = false;
            }
            continue;
        }

        if (auto it = std::ranges::find_if(heap_allocations, [&allocation](auto const& entry) { return entry.second.contains_address(allocation.pointer); });
            it == heap_allocations.end())
        {
            if(allocation.allocated)
            {
                auto it_closest = heap_allocations.lower_bound(allocation.pointer);
                if (it_closest != heap_allocations.begin())
                {
                    --it_closest;
                }

                // this happens when the base dmp has a allocation address/size that is reused between the first dmp and the second dump and
                // we don't have any record of it and it's not a crt heap (i.e. we can't tell they are different by the request number) then
                // we can't do anything about it as it looks the same... the only reason we can spot this unexpected situation is because
                // we have the json report of what you expected to have allocated.
                if(!crt_heap.is_using_crt_heap() && heaps.is_address_filtered(allocation.pointer, bytes{allocation.size}))
                {
                    *o_log << "WARNING: [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] is a filtered entry, the address/size is reused from the base dmp file\n";
                }
                else
                {
                    *o_log << "ERROR: can't find json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "], closest dmp heap allocation is [" << dlg_help_utils::stream_hex_dump::to_hex(it_closest->first, hex_length) <<"] of [" << it_closest->second.user_requested_size().count() << "]\n";
                    successful = false;
                }
            }
        }
        else if(!allocation.allocated)
        {
            *o_log << "INFO: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] even though it's free(!), found dmp heap allocation is [" << dlg_help_utils::stream_hex_dump::to_hex(it->first, hex_length) <<"] of [" << it->second.user_requested_size().count() << "]\n";
        }
        else if(allocation.pointer != it->second.user_address())
        {
            *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] but dmp heap allocation pointer is different [" << dlg_help_utils::stream_hex_dump::to_hex(it->first, hex_length) <<"] of [" << it->second.user_requested_size().count() << "]\n";
            successful = false;
        }
        else if(allocation.size != static_cast<size_t>(it->second.user_requested_size().count()))
        {
            *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] but dmp heap allocation size is different [" << it->second.user_requested_size().count() << "]\n";
            successful = false;
        }
        else if(stacktrace && it->second.allocation_stack_trace().empty())
        {
            *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] but dmp heap allocation has no expected stack trace\n";
            successful = false;
        }
        else
        {
            size_t length{0};
            for(auto stream = it->second.user_data();
                auto [range_buffer, range_length] : stream.ranges())
            {
                auto* start_data = static_cast<char const*>(range_buffer);
                auto* data = start_data;
                auto* end_data = data + range_length;
                while(data < end_data)
                {
                    if(*data != allocation.fill_char)
                    {
                        *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] but failed fill compare of [" << allocation.fill_char << "] compared to dmp allocation of [" << *data << "] @ [" << dlg_help_utils::stream_hex_dump::to_hex(length + (data - start_data)) << "]\n";
                        successful = false;
                        break;
                    }
                    ++data;
                }
                length += range_length;
            }
        }
    }

    // valid CRT Entries are all found
    for (auto const& entry : crt_heap.entries())
    {
        if(entry.block_use())
        {
            if(heap_allocations.find(entry.user_address()) == heap_allocations.end())
            {
                *o_log << "ERROR: CRT Allocation Entry [" << dlg_help_utils::stream_hex_dump::to_hex(entry.user_address()) << "] of [" << entry.data_size() << "] not found\n";
                successful = false;
            }
        }
        else
        {
            if(heap_free_entries.find(entry.user_address()) == heap_free_entries.end())
            {
                *o_log << "ERROR: CRT Free Entry [" << dlg_help_utils::stream_hex_dump::to_hex(entry.user_address()) << "] of [" << entry.data_size() << "] not found\n";
                successful = false;
            }
        }
    }

    std::wcout << dump_filename << " : ";
    auto console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console, successful ? ConsoleForeground::GREEN : ConsoleForeground::RED);
    std::wcout << (successful ? "Passed" : "Failed");
    SetConsoleTextAttribute(console, ConsoleForeground::WHITE);
    std::wcout << '\n';

    return successful;
}

int main(int const argc, char* argv[])
{
    try
    {
        try
        {
            std::string dump_filename_1_l;
            std::string dump_filename_2_l;
            std::string log_filename_l;
            std::string json_filename_l;
            auto stacktrace{false};
            auto show_help{false};
            auto cli = lyra::help(show_help)
                | lyra::opt( dump_filename_1_l, "filename" )["-1"]["--dmp1"]("first dump filename")
                | lyra::opt( dump_filename_2_l, "filename" )["-2"]["--dmp2"]("second dump filename")
                | lyra::opt( log_filename_l, "filename" )["-l"]["--log"]("log filename")
                | lyra::opt( json_filename_l, "filename" )["-j"]["--json"]("json filename")
                | lyra::opt( stacktrace)["-s"]["--stacktrace"]("expects allocation stack trace")
                ;

            if (auto const result = cli.parse({ argc, argv });
                !result)
            {
                std::cerr << "Error in command line: " << result.errorMessage() << '\n';
                std::cerr << cli << "\n";
                return EXIT_FAILURE;
            }

            // Show the help when asked for.
            if (show_help)
            {
                std::cout << cli << '\n';
                return EXIT_SUCCESS;
            }

            auto const dump_filename_1 = dlg_help_utils::string_conversation::acp_to_wstring(dump_filename_1_l);
            auto const dump_filename_2 = dlg_help_utils::string_conversation::acp_to_wstring(dump_filename_2_l);
            auto const log_filename = dlg_help_utils::string_conversation::acp_to_wstring(log_filename_l);
            auto const json_filename = dlg_help_utils::string_conversation::acp_to_wstring(json_filename_l);

            std::unique_ptr<std::wfstream> log;
            std::wostream* o_log{&std::wcout};
            if(!log_filename.empty())
            {
                log = std::make_unique<std::wfstream>(log_filename, std::ios_base::out | std::ios_base::app);
                if(log->bad())
                {
                    std::wcerr << "failed to open log file: " << log_filename << '\n';
                    return EXIT_FAILURE;
                }
                o_log = log.get();
            }

            std::fstream json_file{json_filename, std::ios_base::in};
            if(json_file.bad())
            {
                *o_log << "failed to open json result set file: " << json_filename << '\n';
                return EXIT_FAILURE;
            }

            std::stringstream buffer;
            buffer << json_file.rdbuf();
            json_file.close();
            auto json = std::move(buffer).str();

            JS::ParseContext context(json);
            ResultSet set;
            if (context.parseTo(set) != JS::Error::NoError)
            {
                *o_log << "failed to parse json result set file: " << json_filename << " with " << dlg_help_utils::string_conversation::acp_to_wstring(context.makeErrorString()) << '\n';
                return EXIT_FAILURE;
            }

            auto successful = validate_dump_file(stacktrace, dump_filename_1, {}, o_log, set.first_allocations);
            if(successful)
            {
                successful = validate_dump_file(stacktrace, dump_filename_2, dump_filename_1, o_log, set.second_allocations);
            }

            if(log)
            {
                log->close();
            }

            return successful ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        catch (std::exception const& e)
        {
            std::cout << "fatal error: " << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "fatal error: Unknown exception\n";
        }
    }
    catch(...)
    {
    }
    return EXIT_FAILURE;
}
