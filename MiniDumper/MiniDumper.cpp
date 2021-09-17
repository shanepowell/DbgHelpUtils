// MiniDumper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <filesystem>
#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4100 4458)
#include <lyra/lyra.hpp>
#pragma warning(pop)

#include "dump_file_options.h"
#include "dump_mini_dump.h"
#include "DbgHelpUtils/filesystem_utils.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/wide_runtime_error.h"

using namespace std;
using namespace dlg_help_utils::exceptions;
using namespace dlg_help_utils;

int main(int const argc, char* argv[])
{
    try
    {
        bool show_help = false;

        // The parser with the multiple option arguments and help option.
        dump_file_options dump_options;
        auto cli = lyra::cli() | lyra::help(show_help) | dump_options.generate_options();

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

        if (dump_options.display_version())
        {
            display_version_information();
            return EXIT_SUCCESS;
        }

        dump_options.process_raw_options();

        if (dump_options.dump_files().empty())
        {
            std::cout << "no dump files specified\n";
            std::cout << cli << '\n';
            return EXIT_SUCCESS;
        }

        for (auto const& dump_file_match : dump_options.dump_files())
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
