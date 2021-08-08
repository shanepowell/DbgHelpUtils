// MiniDumper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <filesystem>
#include <iostream>

#include <boost/program_options.hpp>

#include "dump_file_options.h"
#include "dump_mini_dump.h"
#include "DbgHelpUtils/filesystem_utils.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/wide_runtime_error.h"

using namespace std;
using namespace dlg_help_utils::exceptions;
using namespace dlg_help_utils;

int wmain(int argc, wchar_t* argv[]);

int wmain(int const argc, wchar_t* argv[])
{
    namespace po = boost::program_options;

    try
    {
        po::options_description options;
        options.add_options()
            ("help,h", "produce help message")
            ("version,v", "display version information")
            ("dumpfile,d", po::wvalue<vector<wstring>>(), "dump files to open")
            ("continue,c", "continue on errors")
            ("header,r", "dump file header")
            ("streams,s", "dump streams")
            ("hexdump,x", "hex dump stream data")
            ("memoryhexdump,m", "hex dump memory data")
            ("streamindex,i", po::wvalue<vector<size_t>>(), "dump stream indexes")
            ("streamtype,t", po::wvalue<vector<wstring>>(), "dump stream types")
            ("symbols,y", "display stack trace symbols")
            ("filter", po::wvalue<vector<wstring>>(), "filter by supported values")
            ("symboldebug", "debug load symbols")
            ("symboldebugmemory", "debug load symbols memory loading")
            ("crc32", "generate crc32")
            ("type,p", po::wvalue<vector<wstring>>(), "dump symbol type information")
            ("symbol", po::wvalue<vector<wstring>>(), "dump symbol information")
            ("moduletypes", po::wvalue<vector<wstring>>(), "dump module symbol types")
            ("address", po::wvalue<vector<wstring>>(), "dump address with type")
            ("typedebug", "debug type data")
            ("peb", "process environment block")
            ("heap", "heap data information")
            ;

        po::variables_map vm;
        store(parse_command_line(argc, argv, options), vm);
        notify(vm);

        if (vm.count("version"))
        {
            display_version_information();
            return EXIT_SUCCESS;
        }

        if (vm.count("help") || !vm.count("dumpfile"))
        {
            cout << options << "\n";
            return EXIT_SUCCESS;
        }

        dump_file_options const dump_options{vm};

        for (auto const& dump_file_match : vm["dumpfile"].as<vector<wstring>>())
        {
            std::wstring match;
            std::filesystem::path path{dump_file_match};
            if (exists(path))
            {
                if (is_regular_file(path))
                {
                    if (!process_dump_file(dump_file_match, dump_options) && !dump_options.continue_on_errors())
                    {
                        return EXIT_FAILURE;
                    }
                    continue;
                }

                if (!is_directory(path))
                {
                    wcout << L"Invalid dump file: [" << path.wstring() << L"]\n";
                    continue;
                }

                match = L"*";
            }
            else
            {
                match = path.filename().wstring();
                path = path.parent_path();

                if (match.find_first_of(L"*?") == std::wstring::npos)
                {
                    wcout << L"Invalid wildcard pattern: [" << match << L"]\n";
                    continue;
                }
            }

            if (path.empty())
            {
                path = ".";
            }

            if (!is_directory(path))
            {
                wcout << L"Invalid directory: [" << path.wstring() << L"]\n";
                continue;
            }

            for (auto const& entry : filesystem::directory_iterator(path))
            {
                if (entry.is_regular_file() &&
                    filesystem_utils::wildcard_match(entry.path().filename().wstring(), match) &&
                    (!process_dump_file(entry.path().wstring(), dump_options) && !dump_options.continue_on_errors()))
                {
                    return EXIT_FAILURE;
                }
            }
        }

        return EXIT_SUCCESS;
    }
    catch (wide_runtime_error const& e)
    {
        wcout << L"fatal error: " << e.message() << L'\n';
    }
    catch (exception const& e)
    {
        wcout << L"fatal error: " << e.what() << L'\n';
    }
    catch (...)
    {
        wcout << L"fatal error: Unknown exception\n";
    }
    return EXIT_FAILURE;
}
