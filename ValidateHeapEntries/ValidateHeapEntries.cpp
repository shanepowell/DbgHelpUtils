// ValidateHeapEntries.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#define NOMINMAX 1
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable : 26812 26495)
#include <boost/program_options.hpp>
#pragma warning(pop)

#include "ResultSet.h"
#include "symbol_engine_ui.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/process_heaps.h"
#include "DbgHelpUtils/process_heap_entry.h"
#include "DbgHelpUtils/stream_hex_dump.h"

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


int main(int const argc, char* argv[])
{
    namespace po = boost::program_options;
    try
    {
        try
        {
            po::options_description options;
            options.add_options()
                ("help,h", "produce help message")
                ("dmp", po::wvalue<std::wstring>(), "dump filename")
                ("log", po::wvalue<std::wstring>(), "log filename")
                ("json", po::wvalue<std::wstring>(), "json filename")
                ("stacktrace", "expects allocation stack trace")
                ;

            po::variables_map vm;
            store(parse_command_line(argc, argv, options), vm);
            notify(vm);

            if (vm.count("help") || vm.count("dmp") == 0 || vm.count("json") == 0)
            {
                std::cout << options << "\n";
                return EXIT_SUCCESS;
            }

            auto const dump_filename = vm["dmp"].as<std::wstring>();
            auto const log_filename = vm.count("log") > 0 ? vm["log"].as<std::wstring>() : std::wstring{};
            auto const json_filename = vm["json"].as<std::wstring>();
            auto const stacktrace = vm.count("stacktrace") > 0;

            std::unique_ptr<std::wfstream> log;
            std::wostream* o_log{&std::wcout};
            if(!log_filename.empty())
            {
                log = std::make_unique<std::wfstream>(log_filename, std::ios_base::out | std::ios_base::app);
                if(log->bad())
                {
                    std::wcout << "failed to open log file: " << log_filename << '\n';
                    return EXIT_FAILURE;
                }
                o_log = log.get();
            }

            auto result = EXIT_SUCCESS;
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
            context.parseTo(set);

            auto successful = true;

            dlg_help_utils::mini_dump dump_file{dump_filename};
            dump_file.open_mini_dump();
            if(dump_file.type() != dlg_help_utils::dump_file_type::user_mode_dump)
            {
                *o_log << "ERROR: invalid dmp file [" << dump_filename << "]\n";
                successful = false;
            }
            else
            {
                *o_log << "Processing dmp [" << dump_filename << "]\n";

                symbol_engine_ui ui;
                dlg_help_utils::dbg_help::symbol_engine symbol_engine{ui};

                std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> heap_allocations;
                dlg_help_utils::heap::process_heaps const heaps{dump_file, symbol_engine};
                auto const hex_length = heaps.peb().machine_hex_printable_length();
                for(auto const& entry : heaps.entries())
                {
                    heap_allocations.insert(std::make_pair(entry.user_address(), entry));
                }


                for(auto const& allocation : set.allocations)
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
                            *o_log << "ERROR: can't find json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "], closest dmp heap allocation is [" << dlg_help_utils::stream_hex_dump::to_hex(it_closest->first, hex_length) <<"] of [" << it_closest->second.user_requested_size().count() << "]\n";
                            successful = false;
                        }
                    }
                    else if(!allocation.allocated)
                    {
                        *o_log << "WARNING: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] even though it's free(!), found dmp heap allocation is [" << dlg_help_utils::stream_hex_dump::to_hex(it->first, hex_length) <<"] of [" << it->second.user_requested_size().count() << "]\n";
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
                        auto stream = it->second.user_data();
                        while(!stream.eof())
                        {
                            char ch;
                            if(stream.read(&ch, sizeof ch) != sizeof ch)
                            {
                                *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] but failed to read dmp heap allocation\n";
                                successful = false;
                                break;
                            }

                            if(ch != allocation.fill_char)
                            {
                                *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] but failed fill compare of [" << allocation.fill_char << "] compared to dmp allocation of [" << ch << "] @ [" << dlg_help_utils::stream_hex_dump::to_hex(stream.current_address() - sizeof ch, hex_length) << "]\n";
                                successful = false;
                                break;
                            }
                        }
                    }
                }
            }

            if(log)
            {
                log->close();
            }

            std::wcout << dump_filename << " : ";
            auto console = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(console, successful ? ConsoleForeground::GREEN : ConsoleForeground::RED);
            std::wcout << (successful ? "Passed" : "Failed");
            SetConsoleTextAttribute(console, ConsoleForeground::WHITE);
            std::wcout << '\n';
            return result;
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
