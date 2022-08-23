#include "dump_mini_dump.h"

#include "dump_file_options.h"
#include "dump_mini_dump_heap.h"
#include "dump_mini_dump_streams.h"
#include "dump_mini_dump_symbols.h"
#include "symbol_engine_ui.h"
#include "DbgHelpUtils/cache_manager.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/mini_dump_stream_type.h"
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

namespace
{
    std::unique_ptr<mini_dump> get_base_diff_dump_file(std::wostream& log, std::wstring const& path)
    {
        if(path.empty())
        {
            return {};
        }

        log << std::format(L"Loading base diff dump file: {}\n", path);
        auto dump_file = std::make_unique<mini_dump>(path);

        dump_file->open_mini_dump();

        if (dump_file->header() == nullptr)
        {
            throw wide_runtime_error{std::format(L"no valid header detected for [{}]", path)};
        }

        if(dump_file->type() != dump_file_type::user_mode_dump)
        {
            throw wide_runtime_error{std::format(L"base diff dump file not a user mode dump for [{}]", path)};
        }

        return dump_file;
    }
}

bool process_dump_file(std::wostream& log, wstring const& file_name, std::wstring const& base_diff_file_name, dump_file_options& options)
{
    try
    {
        log << std::format(L"Loading dump file: {}\n", file_name);

        mini_dump dump_file{file_name};

        try
        {
            dump_file.open_mini_dump();
        }
        catch (wide_runtime_error const& e)
        {
            log << std::format(L"error opening [{0}]: {1}\n", file_name, e.message());
        }

        if (dump_file.header() == nullptr)
        {
            log << std::format(L"no valid header detected for [{}]\n", file_name);
            return false;
        }

        switch (dump_file.type())
        {
        case dump_file_type::user_mode_dump:
            process_user_mode_dump(log, dump_file, get_base_diff_dump_file(log, base_diff_file_name), options);
            break;

        case dump_file_type::invalid:
            process_invalid_user_mode_dump(log, dump_file, options);
            break;

        case dump_file_type::kernel_mode_dump_x86:
            process_x86_kernel_memory_dump(log, dump_file, options);
            break;

        case dump_file_type::kernel_mode_dump_x64:
            process_x64_kernel_memory_dump(log, dump_file, options);
            break;
        }

        return true;
    }
    catch (wide_runtime_error const& e)
    {
        log << std::format(L"fatal error processing [{0}]: {1}\n", file_name, e.message());
    }
    catch (exception const& e)
    {
        log << std::format(L"fatal error processing [{0}]: {1}\n", file_name, string_conversation::acp_to_wstring(e.what()));
    }
    catch (...)
    {
        log << std::format(L"fatal error processing [{}]:  Unknown exception\n", file_name);
    }
    return false;
}

void process_user_mode_dump(std::wostream& log, mini_dump const& dump_file, std::unique_ptr<mini_dump> const& base_diff_dump, dump_file_options& options)
{
    cache_manager cache;
    symbol_engine_ui ui{options};
    dbg_help::symbol_engine symbol_engine{ui};
    if (options.dump_header())
    {
        dump_mini_dump_header(log, dump_file, options);
    }

    if (options.dump_streams())
    {
        dump_mini_dump_streams(log, dump_file);
    }

    if(options.dump_all_stream_indexes())
    {
        dump_mini_dump_all_stream_indexes(log, dump_file, options, symbol_engine);
    }
    else
    {
        for (auto const index : options.dump_stream_indexes())
        {
            try
            {
                dump_mini_dump_stream_index(log, dump_file, index, options, symbol_engine);
            }
            catch (wide_runtime_error const& e)
            {
                throw wide_runtime_error{std::format(L"Failure to dump stream index: [{}]", index), e};
            }
            catch (exception const& e)
            {
                throw wide_runtime_error{std::format(L"Failure to dump stream index: [{}]", index), e};
            }
        }

        for (auto const& type : options.dump_stream_types())
        {
            try
            {
                dump_mini_dump_stream_type(log, dump_file, type, options, symbol_engine);
            }
            catch (wide_runtime_error const& e)
            {
                throw wide_runtime_error{std::format(L"Failure to dump stream type: [{}]", mini_dump_stream_type::to_wstring(type)), e};
            }
            catch (exception const& e)
            {
                throw wide_runtime_error{std::format(L"Failure to dump stream type: [{}]", mini_dump_stream_type::to_wstring(type)), e};
            }
        }
    }

    if(options.display_peb())
    {
        dump_mini_dump_peb(log, dump_file, cache, options, symbol_engine);
    }

    if(options.display_heap())
    {
        dump_mini_dump_heap(log, dump_file, cache, options, symbol_engine);
    }

    if(options.display_heap_entries())
    {
        dump_mini_dump_heap_entries(log, dump_file, cache, base_diff_dump, options, symbol_engine);
    }

    if(options.display_crtheap())
    {
        dump_mini_dump_crtheap(log, dump_file, cache, base_diff_dump, options, symbol_engine);
    }

    if(options.display_heap_statistic_views())
    {
        dump_mini_dump_heap_statistics(log, dump_file, cache, base_diff_dump, options, symbol_engine);
    }

    if(options.display_heap_graph())
    {
        dump_mini_dump_heap_graph(log, dump_file, cache, options, symbol_engine);
    }

    if(options.display_stack_trace_database())
    {
        dump_mini_dump_stack_trace_database(log, dump_file, cache, options, symbol_engine);
    }

    for (auto const& module_name : options.dump_types_modules())
    {
        try
        {
            dump_mini_dump_module_symbol_types(log, dump_file, module_name, options, symbol_engine);
        }
        catch (wide_runtime_error const& e)
        {
            throw wide_runtime_error{std::format(L"Failure to dump module: [{}]", module_name), e};
        }
        catch (exception const& e)
        {
            throw wide_runtime_error{std::format(L"Failure to dump module: [{}]", module_name), e};
        }
    }

    for (auto const& type : options.symbol_types())
    {
        try
        {
            dump_mini_dump_symbol_type(log, dump_file, type, options, symbol_engine);
        }
        catch (wide_runtime_error const& e)
        {
            throw wide_runtime_error{std::format(L"Failure to dump type: [{}]", type), e};
        }
        catch (exception const& e)
        {
            throw wide_runtime_error{std::format(L"Failure to dump type: [{}]", type), e};
        }
    }

    for (auto const& symbol_name : options.symbol_names())
    {
        try
        {
            dump_mini_dump_symbol_name(log, dump_file, symbol_name, options, symbol_engine);
        }
        catch (wide_runtime_error const& e)
        {
            throw wide_runtime_error{std::format(L"Failure to dump symbol: [{}]", symbol_name), e};
        }
        catch (exception const& e)
        {
            throw wide_runtime_error{std::format(L"Failure to dump symbol: [{}]", symbol_name), e};
        }
    }

    for (auto const& address_type : options.dump_address_types())
    {
        try
        {
            dump_mini_dump_address(log, dump_file, address_type, options, symbol_engine);
        }
        catch (wide_runtime_error const& e)
        {
            throw wide_runtime_error{std::format(L"Failure to dump address: [{}]", address_type), e};
        }
        catch (exception const& e)
        {
            throw wide_runtime_error{std::format(L"Failure to dump address: [{}]", address_type), e};
        }
    }
}

void process_invalid_user_mode_dump(std::wostream& log, mini_dump const& dump_file, dump_file_options const& options)
{
    dump_mini_dump_header(log, dump_file, options);
}

void process_x86_kernel_memory_dump(std::wostream& log, mini_dump const& dump_file, dump_file_options const& options)
{
    dump_mini_dump_header(log, dump_file, options);
}

void process_x64_kernel_memory_dump(std::wostream& log, mini_dump const& dump_file, dump_file_options const& options)
{
    dump_mini_dump_header(log, dump_file, options);
}

void dump_mini_dump_header(std::wostream& log, mini_dump const& dump_file, dump_file_options const& options)
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
        log << L"MINIDUMP_HEADER:\n";
        log << std::format(L" Signature [{0}] - {1}\n", dump_file.version_string(), dump_file.is_valid() ? L"Valid" : L"Invalid");
        log << std::format(L" Version [{0}] (internal: {1})\n", LOWORD(header->Version), HIWORD(header->Version));
        break;
    case dump_file_type::kernel_mode_dump_x86:
        log << L"X86 Kernel Memory Dump:\n";
        log << std::format(L" Signature [{0}] - {1}\n", dump_file.version_string(), dump_file.is_valid() ? L"Valid" : L"Invalid");
        return;
    case dump_file_type::kernel_mode_dump_x64:
        log << L"X64 Kernel Memory Dump:\n";
        log << std::format(L" Signature [{0}] - {1}\n", dump_file.version_string(), dump_file.is_valid() ? L"Valid" : L"Invalid");
        return;
    case dump_file_type::invalid:
    default: // NOLINT(clang-diagnostic-covered-switch-default)
        // ReSharper disable once StringLiteralTypo
        log << L"MINIDUMP_HEADER:\n";
        log << std::format(L" Signature [{0}] - {1}\n", to_hex(header->Signature), dump_file.is_valid() ? L"Valid" : L"Invalid");
        log << std::format(L" Version [{0}] (internal: {1})\n", LOWORD(header->Version), HIWORD(header->Version));
        return;
    }
    log << std::format(L" Dump Type [{}]\n", system_info_utils::mini_dump_type_to_string(dump_file.type()));
    log << std::format(L" NumberOfStreams [{}]\n", locale_formatting::to_wstring(header->NumberOfStreams));
    log << std::format(L" StreamDirectoryRva [{}]\n", to_hex(header->StreamDirectoryRva));
    log << std::format(L" CheckSum [{}]", to_hex(header->CheckSum));;
    if (options.generate_crc32())
    {
        log << std::format(L" (crc32:{})", to_hex(dump_file.data_crc32()));
    }
    log << L'\n';
    log << std::format(L" Timestamp [local: {0}] [UTC: {1}]\n", time_utils::to_local_time(header->TimeDateStamp), time_utils::to_utc_time(header->TimeDateStamp));
    log << std::format(L" Flags [{}] - \n", to_hex(header->Flags));
    for (auto const& type : mini_dump_type::to_strings(static_cast<MINIDUMP_TYPE>(header->Flags)))
    {
        log << std::format(L"  [{}]\n", type);
    }
}

void display_version_information(std::wostream& log)
{
    auto const& path = system_info_utils::get_running_application_path();
    log << std::format(L"MiniDumper Version: {}\n", system_info_utils::get_product_version(path.wstring()));
    log << L'\n';

    OSVERSIONINFOEXW info{};
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
#pragma warning(push)
#pragma warning(disable: 4996 28159)
    // ReSharper disable CppDeprecatedEntity
    if (!GetVersionExW(reinterpret_cast<OSVERSIONINFOW*>(&info))) // NOLINT(clang-diagnostic-deprecated-declarations)
    {
        auto const ec = GetLastError();
        log << std::format(L"GetVersionExW failed: {0} - {1}\n", to_hex(ec), windows_error::get_windows_error_string(ec));
        return;
    }
    // ReSharper restore CppDeprecatedEntity
#pragma warning(pop)

    SYSTEM_INFO system_info{};
    GetSystemInfo(&system_info);

    log << std::format(L"OS Version: {0}.{1}.{2} - {3}\n", info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber, system_info_utils::windows_version_to_string(info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber, info.wProductType, system_info.wProcessorArchitecture, info.wSuiteMask));
    log << std::format(L"  SuiteMask: {}\n", to_hex(info.wSuiteMask));
    for (auto const& value : system_info_utils::suite_mask_to_strings(info.wSuiteMask))
    {
        log << std::format(L"    {}\n", value);
    }
    log << std::format(L"  ProductType: {}\n", system_info_utils::product_type_to_string(info.wProductType));
    log << std::format(L"  PlatformId: {}\n", system_info_utils::platform_id_to_string(info.dwPlatformId));
    log << std::format(L"  CSDVersion: {}\n", info.szCSDVersion);
    log << std::format(L"  ProcessorArchitecture: {}\n", system_info_utils::processor_architecture_to_string(system_info.wProcessorArchitecture));
}
