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
        const auto cli = lyra::cli() | lyra::help(show_help) | dump_options.generate_options();

        if (auto const result = cli.parse({ argc, argv });
            !result)
        {
            std::cerr << std::format("Error in command line: {}\n", result.errorMessage());
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
            display_version_information(wcout);
            return EXIT_SUCCESS;
        }

        dump_options.process_raw_options();

        if (dump_options.dump_files().empty())
        {
            std::cout << "no dump files specified\n";
            std::cout << cli << '\n';
            return EXIT_SUCCESS;
        }

        auto& log = dump_options.get_log_stream();

        auto const on_error_handler = [&dump_options, &log](std::wstring const& error){ log << std::format(L"Dump file: {}\n", error); return dump_options.continue_on_errors(); };
        // ReSharper disable once CppLocalVariableMayBeConst
        auto base_diff_dump_files = filesystem_utils::enumerate_files(dump_options.base_diff_dump_files(), on_error_handler);
        auto base_diff_dump_files_current = std::begin(base_diff_dump_files);
        auto const base_diff_dump_files_end = std::end(base_diff_dump_files);
        for (auto const& dump_file : filesystem_utils::enumerate_files(dump_options.dump_files(), on_error_handler))
        {
            std::wstring base_diff_dump_file;
            if(base_diff_dump_files_current != base_diff_dump_files_end)
            {
                base_diff_dump_file = *base_diff_dump_files_current;
                ++base_diff_dump_files_current;
            }

            if(!process_dump_file(log, dump_file, base_diff_dump_file, dump_options) && !dump_options.continue_on_errors())
            {
                return EXIT_FAILURE;
            }
        }

        if(base_diff_dump_files_current != base_diff_dump_files_end)
        {
            log << L"warning: more base diff dump file paths than dump file paths\n";
        }

        return EXIT_SUCCESS;
    }
    catch (wide_runtime_error const& e)
    {
        wcerr << std::format(L"fatal error: {}\n", e.message());
    }
    catch (exception const& e)
    {
        wcerr << std::format(L"fatal error: {}\n", string_conversation::acp_to_wstring(e.what()));
    }
    catch (...)
    {
        wcerr << L"fatal error: Unknown exception\n";
    }
    return EXIT_FAILURE;
}
