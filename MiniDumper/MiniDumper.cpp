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

        // ReSharper disable once CppLocalVariableMayBeConst
        auto base_diff_dump_files = filesystem_utils::enumerate_files(dump_options.base_diff_dump_files(), [&dump_options](std::wstring const& error){ wcout << L"Dump file: " << error << L'\n'; return dump_options.continue_on_errors(); });
        auto base_diff_dump_files_current = std::begin(base_diff_dump_files);
        auto const base_diff_dump_files_end = std::end(base_diff_dump_files);
        for (auto const& dump_file : filesystem_utils::enumerate_files(dump_options.dump_files(), [&dump_options](std::wstring const& error){ wcout << L"Dump file: " << error << L'\n'; return dump_options.continue_on_errors(); }))
        {
            std::wstring base_diff_dump_file;
            if(base_diff_dump_files_current != base_diff_dump_files_end)
            {
                base_diff_dump_file = *base_diff_dump_files_current;
                ++base_diff_dump_files_current;
            }

            if(!process_dump_file(dump_file, base_diff_dump_file, dump_options) && !dump_options.continue_on_errors())
            {
                return EXIT_FAILURE;
            }
        }

        if(base_diff_dump_files_current != base_diff_dump_files_end)
        {
            wcout << "warning: more base diff dump file paths than dump file paths\n";
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
