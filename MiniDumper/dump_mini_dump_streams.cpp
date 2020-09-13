#include "dump_mini_dump_streams.h"

#include <iostream>

#include "dump_file_options.h"
#include "dump_mini_dump_app_info.h"
#include "dump_mini_dump_comment.h"
#include "dump_mini_dump_exception.h"
#include "dump_mini_dump_info.h"
#include "dump_mini_dump_memory.h"
#include "dump_mini_dump_module.h"
#include "dump_mini_dump_thread.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/mini_dump_stream_type.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/wide_runtime_error.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils::exceptions;
using namespace dlg_help_utils;

void dump_mini_dump_streams(mini_dump const& dump_file)
{
    auto const* header = dump_file.header();
    if (header == nullptr)
    {
        return;
    }

    auto const* directory = dump_file.directory();
    if (directory == nullptr)
    {
        return;
    }

    wcout << L"STREAM Directory:\n";
    for (size_t index = 0; index < header->NumberOfStreams; ++index)
    {
        using namespace size_units::base_10;
        auto const& entry = directory[index];
        wcout << L"Stream: [" << index << "] " <<
            mini_dump_stream_type::to_string(static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType)) << " - RVA: [" <<
            entry.Location.Rva << "] - [" << to_hex(entry.Location.DataSize) << L"](" << bytes{entry.Location.DataSize}
            << ")\n";
    }
}

void dump_mini_dump_stream_index(mini_dump const& dump_file, size_t const index, dump_file_options const& options,
                                 dbg_help::symbol_engine& symbol_engine)
{
    auto const* header = dump_file.header();
    if (header == nullptr)
    {
        return;
    }

    auto const* directory = dump_file.directory();
    if (directory == nullptr)
    {
        return;
    }

    if (index >= header->NumberOfStreams)
    {
        throw wide_runtime_error{(std::wostringstream{} << "stream index [" << index << "] out of range").str()};
    }

    auto const& entry = directory[index];
    dump_mini_dump_stream_data(dump_file, index, entry, options, symbol_engine);
}

void dump_mini_dump_stream_type(mini_dump const& dump_file, MINIDUMP_STREAM_TYPE const type,
                                dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    auto const* header = dump_file.header();
    if (header == nullptr)
    {
        return;
    }

    auto const* directory = dump_file.directory();
    if (directory == nullptr)
    {
        return;
    }

    for (size_t index = 0; index < header->NumberOfStreams; ++index)
    {
        auto const& entry = directory[index];
        if (static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType) == type)
        {
            dump_mini_dump_stream_data(dump_file, index, entry, options, symbol_engine);
            return;
        }
    }

    throw wide_runtime_error{
        (std::wostringstream{} << "stream type [" << mini_dump_stream_type::to_string(type) << "] not found").str()
    };
}


void dump_mini_dump_stream_data(mini_dump const& mini_dump, size_t const index, MINIDUMP_DIRECTORY const& entry,
                                dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    auto hex_dump_stream = false;
    auto const type = static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType);
    wcout << mini_dump_stream_type::to_string(type) << L":\n";
    switch (type)
    {
    case UnusedStream:
        break;
    case ReservedStream0:
    case ReservedStream1:
    case JavaScriptDataStream:
    case ceStreamNull:
    case ceStreamSystemInfo:
    case ceStreamException:
    case ceStreamModuleList:
    case ceStreamProcessList:
    case ceStreamThreadList:
    case ceStreamThreadContextList:
    case ceStreamThreadCallStackList:
    case ceStreamMemoryVirtualList:
    case ceStreamMemoryPhysicalList:
    case ceStreamBucketParameters:
    case ceStreamProcessModuleMap:
    case ceStreamDiagnosisList:
    case LastReservedStream:
        hex_dump_stream = true;
        wcout << L"Unsupported data type\n";
        break;
    case ThreadListStream:
        dump_mini_dump_thread_list_stream_data(mini_dump, index, options, symbol_engine);
        break;
    case ModuleListStream:
        dump_mini_dump_module_list_stream_data(mini_dump, index, options);
        break;
    case MemoryListStream:
        dump_mini_dump_memory_list_stream_data(mini_dump, index, options);
        break;
    case ExceptionStream:
        dump_mini_dump_exception_stream_data(mini_dump, index, options, symbol_engine);
        break;
    case SystemInfoStream:
        dump_mini_dump_system_info_stream_data(mini_dump, index);
        break;
    case ThreadExListStream:
        dump_mini_dump_thread_list_ex_stream_data(mini_dump, index, options, symbol_engine);
        break;
    case Memory64ListStream:
        dump_mini_dump_memory64_list_stream_data(mini_dump, index, options);
        break;
    case CommentStreamA:
        dump_mini_dump_comment_a_stream_data(mini_dump, index);
        break;
    case CommentStreamW:
        dump_mini_dump_comment_w_stream_data(mini_dump, index);
        break;
    case HandleDataStream:
        dump_mini_dump_handle_data_stream_data(mini_dump, index, options);
        break;
    case FunctionTableStream:
        dump_mini_dump_function_table_stream_data(mini_dump, index, options);
        break;
    case UnloadedModuleListStream:
        dump_mini_dump_unloaded_module_list_stream_data(mini_dump, index);
        break;
    case MiscInfoStream:
        dump_mini_dump_misc_info_stream_data(mini_dump, index);
        break;
    case MemoryInfoListStream:
        dump_mini_dump_memory_info_list_stream_data(mini_dump, index);
        break;
    case ThreadInfoListStream:
        dump_mini_dump_thread_info_list_stream_data(mini_dump, index);
        break;
    case HandleOperationListStream:
        dump_mini_dump_handle_operation_list_stream_data(mini_dump, index, options, symbol_engine);
        break;
    case TokenStream:
        dump_mini_dump_token_stream_data(mini_dump, index, options);
        break;
    case SystemMemoryInfoStream:
        dump_mini_dump_system_memory_info_stream_data(mini_dump, index);
        break;
    case ProcessVmCountersStream:
        dump_mini_dump_process_vm_counters_stream_data(mini_dump, index);
        break;
    case IptTraceStream:
        dump_mini_dump_ipt_trace_stream_data(mini_dump, index);
        break;
    case ThreadNamesStream:
        dump_mini_dump_thread_names_stream_data(mini_dump, index);
        break;

    default:  // NOLINT(clang-diagnostic-covered-switch-default)
        hex_dump_stream = true;
        if (type < LastReservedStream)
        {
            wcout << L"Unknown Stream Type\n";
        }
        break;
    }

    if (options.hex_dump_stream_data() || hex_dump_stream)
    {
        wcout << L"Data:\n";
        hex_dump::hex_dump(wcout, mini_dump.rva32(entry.Location), entry.Location.DataSize, 1);
        wcout << L'\n';
    }
}
