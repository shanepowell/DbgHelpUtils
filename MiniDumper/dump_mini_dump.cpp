#include "dump_mini_dump.h"

#include <iostream>

#include "dump_file_options.h"
#include "dump_mini_dump_heap.h"
#include "dump_mini_dump_streams.h"
#include "dump_mini_dump_symbols.h"
#include "symbol_engine_ui.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/mini_dump_type.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/symbol_engine.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/time_utils.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "DbgHelpUtils/windows_error.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils::exceptions;
using namespace dlg_help_utils;

bool process_dump_file(wstring const& file_name, dump_file_options const& options)
{
    try
    {
        wcout << L"Loading dump file: "s << file_name << L'\n';

        mini_dump dump_file{file_name};

        try
        {
            dump_file.open_mini_dump();
        }
        catch (wide_runtime_error const& e)
        {
            wcout << L"error opening [" << file_name << "]: " << e.message() << L'\n';
        }

        if (dump_file.header() == nullptr)
        {
            wcout << L"no valid header detected for [" << file_name << L"]\n";
            return false;
        }

        switch (dump_file.type())
        {
        case dump_file_type::user_mode_dump:
            process_user_mode_dump(dump_file, options);
            break;

        case dump_file_type::invalid:
            process_invalid_user_mode_dump(dump_file, options);
            break;

        case dump_file_type::kernel_mode_dump_x86:
            process_x86_kernel_memory_dump(dump_file, options);
            break;

        case dump_file_type::kernel_mode_dump_x64:
            process_x64_kernel_memory_dump(dump_file, options);
            break;
        }

        return true;
    }
    catch (wide_runtime_error const& e)
    {
        wcout << L"fatal error processing [" << file_name << "]: " << e.message() << L'\n';
    }
    catch (exception const& e)
    {
        wcout << L"fatal error processing [" << file_name << "]:" << e.what() << L'\n';
    }
    catch (...)
    {
        wcout << L"fatal error processing [" << file_name << "]:  Unknown exception\n";
    }
    return false;
}

void process_user_mode_dump(mini_dump const& dump_file, dump_file_options const& options)
{
    symbol_engine_ui ui{options};
    dbg_help::symbol_engine symbol_engine{ui};
    if (options.dump_header())
    {
        dump_mini_dump_header(dump_file, options);
    }

    if (options.dump_streams())
    {
        dump_mini_dump_streams(dump_file);
    }

    for (auto const index : options.dump_stream_indexes())
    {
        dump_mini_dump_stream_index(dump_file, index, options, symbol_engine);
    }

    for (auto const& type : options.dump_stream_types())
    {
        dump_mini_dump_stream_type(dump_file, type, options, symbol_engine);
    }

    if(options.display_peb())
    {
        dump_mini_dump_peb(dump_file, options, symbol_engine);
    }

    if(options.display_heap())
    {
        dump_mini_dump_heap(dump_file, options, symbol_engine);
    }

    if(options.display_heap_entries())
    {
        dump_mini_dump_heap_entries(dump_file, options, symbol_engine);
    }

    if(options.display_crtheap())
    {
        dump_mini_dump_crtheap(dump_file, options, symbol_engine);
    }

    if(options.display_stack_trace_database())
    {
        dump_mini_dump_stack_trace_database(dump_file, options, symbol_engine);
    }

    for (auto const& module_name : options.dump_types_modules())
    {
        dump_mini_dump_module_symbol_types(dump_file, module_name, options, symbol_engine);
    }

    for (auto const& type : options.symbol_types())
    {
        dump_mini_dump_symbol_type(dump_file, type, options, symbol_engine);
    }

    for (auto const& symbol_name : options.symbol_names())
    {
        dump_mini_dump_symbol_name(dump_file, symbol_name, options, symbol_engine);
    }

    for (auto const& address_type : options.dump_address_types())
    {
        dump_mini_dump_address(dump_file, address_type, options, symbol_engine);
    }
}

void process_invalid_user_mode_dump(mini_dump const& dump_file, dump_file_options const& options)
{
    dump_mini_dump_header(dump_file, options);
}

void process_x86_kernel_memory_dump(mini_dump const& dump_file, dump_file_options const& options)
{
    dump_mini_dump_header(dump_file, options);
}

void process_x64_kernel_memory_dump(mini_dump const& dump_file, dump_file_options const& options)
{
    dump_mini_dump_header(dump_file, options);
}

void dump_mini_dump_header(mini_dump const& dump_file, dump_file_options const& options)
{
    auto const* header = dump_file.header();
    if (header == nullptr)
    {
        return;
    }

    switch (dump_file.type())
    {
    case dump_file_type::user_mode_dump:
        // ReSharper disable once StringLiteralTypo
        wcout << L"MINIDUMP_HEADER:\n";
        wcout << L" Signature [" << dump_file.version_string() << L"] - " << (dump_file.is_valid()
                                                                                  ? L"Valid"
                                                                                  : L"Invalid") << L'\n';
        wcout << L" Version [" << LOWORD(header->Version) << L"] (internal: " << HIWORD(header->Version) << L")\n";
        break;
    case dump_file_type::kernel_mode_dump_x86:
        wcout << L"X86 Kernel Memory Dump:\n";
        wcout << L" Signature [" << dump_file.version_string() << L"] - " << (dump_file.is_valid()
                                                                                  ? L"Valid"
                                                                                  : L"Invalid") << L'\n';
        return;
    case dump_file_type::kernel_mode_dump_x64:
        wcout << L"X64 Kernel Memory Dump:\n";
        wcout << L" Signature [" << dump_file.version_string() << L"] - " << (dump_file.is_valid()
                                                                                  ? L"Valid"
                                                                                  : L"Invalid") << L'\n';
        return;
    case dump_file_type::invalid:
    default: // NOLINT(clang-diagnostic-covered-switch-default)
        // ReSharper disable once StringLiteralTypo
        wcout << L"MINIDUMP_HEADER:\n";
        wcout << L" Signature [" << to_hex(header->Signature) << L"] - " << (dump_file.is_valid()
                                                                                 ? L"Valid"
                                                                                 : L"Invalid") << L'\n';
        wcout << L" Version [" << LOWORD(header->Version) << L"] (internal: " << HIWORD(header->Version) << L")\n";
        return;
    }
    wcout << L" Dump Type [" << system_info_utils::mini_dump_type_to_string(dump_file.type()) << L"]\n";
    wcout << L" NumberOfStreams [" << header->NumberOfStreams << L"]\n";
    wcout << L" StreamDirectoryRva [" << to_hex(header->StreamDirectoryRva) << L"]\n";
    wcout << L" CheckSum [" << to_hex(header->CheckSum) << L']';
    if (options.generate_crc32())
    {
        wcout << L" (crc32:" << to_hex(dump_file.data_crc32()) << L')';
    }
    wcout << L'\n';
    wcout << L" Timestamp [local: " << time_utils::to_local_time(header->TimeDateStamp) << L"] [UTC: " <<
        time_utils::to_utc_time(header->TimeDateStamp) << L"]\n";
    wcout << L" Flags [" << to_hex(header->Flags) << L"] - \n";
    for (auto const& type : mini_dump_type::to_strings(static_cast<MINIDUMP_TYPE>(header->Flags)))
    {
        wcout << L"  [" << type << L"]\n";
    }
}

void display_version_information()
{
    auto const& path = system_info_utils::get_running_application_path();
    wcout << L"MiniDumper Version: " << system_info_utils::get_product_version(path.wstring()) << L'\n';
    wcout << L'\n';

    OSVERSIONINFOEXW info{};
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
#pragma warning(push)
#pragma warning(disable: 4996 28159)
    // ReSharper disable CppDeprecatedEntity
    if (!GetVersionExW(reinterpret_cast<OSVERSIONINFOW*>(&info))) // NOLINT(clang-diagnostic-deprecated-declarations)
    {
        auto const ec = GetLastError();
        wcout << L"GetVersionExW failed: " << ec << L" - " << windows_error::get_windows_error_string(ec) << L'\n';
        return;
    }
    // ReSharper restore CppDeprecatedEntity
#pragma warning(pop)

    SYSTEM_INFO system_info{};
    GetSystemInfo(&system_info);

    wcout << L"OS Version: " << info.dwMajorVersion << L'.' << info.dwMinorVersion << L'.' << info.dwBuildNumber <<
        L" - " << system_info_utils::windows_version_to_string(info.dwMajorVersion, info.dwMinorVersion,
                                                               info.dwBuildNumber, info.wProductType,
                                                               system_info.wProcessorArchitecture,
                                                               info.wSuiteMask) << L'\n';
    wcout << L"  SuiteMask: " << to_hex(info.wSuiteMask) << L'\n';
    for (auto const& value : system_info_utils::suite_mask_to_strings(info.wSuiteMask))
    {
        wcout << L"    " << value << L'\n';
    }
    wcout << L"  ProductType: " << system_info_utils::product_type_to_string(info.wProductType) << L'\n';
    wcout << L"  PlatformId: " << system_info_utils::platform_id_to_string(info.dwPlatformId) << L'\n';
    wcout << L"  CSDVersion: " << info.szCSDVersion << L'\n';
    wcout << L"  ProcessorArchitecture: " << system_info_utils::processor_architecture_to_string(
        system_info.wProcessorArchitecture) << L'\n';
}
