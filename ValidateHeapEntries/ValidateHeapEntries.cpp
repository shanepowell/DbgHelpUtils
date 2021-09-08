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

            dlg_help_utils::mini_dump dump_file{dump_filename};
            dump_file.open_mini_dump();
            if(dump_file.type() != dlg_help_utils::dump_file_type::user_mode_dump)
            {
                *o_log << "ERROR: invalid dmp file [" << dump_filename << "]\n";
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
                        }
                        continue;
                    }

                    if (auto it = heap_allocations.lower_bound(allocation.pointer);
                        it == heap_allocations.end() || it->first != allocation.pointer)
                    {
                        if(allocation.allocated)
                        {
                            if(it != heap_allocations.begin())
                            {
                                --it;
                            }

                            *o_log << "ERROR: can't find json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "], closest dmp heap allocation is [" << dlg_help_utils::stream_hex_dump::to_hex(it->first, hex_length) <<"] of [" << it->second.user_requested_size().count() << "]\n";
                        }
                    }
                    else if(!allocation.allocated)
                    {
                        *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] even though it's free(!), found dmp heap allocation is [" << dlg_help_utils::stream_hex_dump::to_hex(it->first, hex_length) <<"] of [" << it->second.user_requested_size().count() << "]\n";
                    }
                    else if(allocation.size != static_cast<size_t>(it->second.user_requested_size().count()))
                    {
                        *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] but dmp heap allocation size is different [" << it->second.user_requested_size().count() << "]\n";
                    }
                    else if(stacktrace && it->second.allocation_stack_trace().empty())
                    {
                        *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] but dmp heap allocation has no expected stack trace\n";
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
                                break;
                            }

                            if(ch != allocation.fill_char)
                            {
                                *o_log << "ERROR: found json allocation [" << dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length) << "] of [" << allocation.size << "] but failed fill compare of [" << allocation.fill_char << "] compared to dmp allocation of [" << ch << "] @ [" << dlg_help_utils::stream_hex_dump::to_hex(stream.current_address() - sizeof ch, hex_length) << "]\n";
                                break;
                            }
                        }
                    }
                }
            }

            log->close();
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
