#include "dump_mini_dump_streams.h"

#include "dump_file_options.h"
#include "dump_mini_dump_app_info.h"
#include "dump_mini_dump_comment.h"
#include "dump_mini_dump_exception.h"
#include "dump_mini_dump_info.h"
#include "dump_mini_dump_memory.h"
#include "dump_mini_dump_module.h"
#include "dump_mini_dump_thread.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/mini_dump_stream_type.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_utils.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/wide_runtime_error.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils::exceptions;
using namespace dlg_help_utils;

void dump_mini_dump_streams(std::wostream& log, mini_dump const& dump_file)
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

    log << L"STREAM Directory:\n";
    for (size_t index = 0; index < header->NumberOfStreams; ++index)
    {
        using namespace size_units::base_16;
        auto const& entry = directory[index];
        log << std::format(L"Stream: [{0}] {1} - RVA: [{2}] - [{3}]({4})\n"
            , locale_formatting::to_wstring(index)
            , mini_dump_stream_type::to_string(static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType))
            , locale_formatting::to_wstring(entry.Location.Rva)
            , to_hex(entry.Location.DataSize)
            , to_wstring(bytes{entry.Location.DataSize}));
    }
}

void dump_mini_dump_stream_index(std::wostream& log, mini_dump const& dump_file, size_t const index, dump_file_options const& options,
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
        throw wide_runtime_error{std::format(L"stream index [{}] out of range", index)};
    }

    auto const& entry = directory[index];
    dump_mini_dump_stream_data(log, dump_file, index, entry, options, symbol_engine);
}

void dump_mini_dump_stream_type(std::wostream& log, mini_dump const& dump_file, MINIDUMP_STREAM_TYPE const type,
                                dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    auto const stream = stream_utils::find_stream_for_type(dump_file, type);
    if(!stream.has_value())
    {
        throw wide_runtime_error{std::format(L"stream type [{}] not found", mini_dump_stream_type::to_string(type))};
    }

    auto const& [index, entry] = stream.value();
    dump_mini_dump_stream_data(log, dump_file, index, *entry, options, symbol_engine);
}

void dump_mini_dump_all_stream_indexes(std::wostream& log, mini_dump const& dump_file, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
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
        using namespace size_units::base_16;
        auto const& entry = directory[index];
        log << std::format(L"Stream: [{0}] {1} - RVA: [{2}] - [{3}]({4}):\n"
            , locale_formatting::to_wstring(index)
            , mini_dump_stream_type::to_string(static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType))
            , locale_formatting::to_wstring(entry.Location.Rva)
            , to_hex(entry.Location.DataSize)
            , to_wstring(bytes{entry.Location.DataSize}));
        dump_mini_dump_stream_index(log, dump_file, index, options, symbol_engine);
    }
}

void dump_mini_dump_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index, MINIDUMP_DIRECTORY const& entry,
                                dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    auto hex_dump_stream = false;
    auto const type = static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType);
    log << std::format(L"{}:\n", mini_dump_stream_type::to_string(type));
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
        log << L"Unsupported data type\n";
        break;
    case ThreadListStream:
        dump_mini_dump_thread_list_stream_data(log, mini_dump, index, options, symbol_engine);
        break;
    case ModuleListStream:
        dump_mini_dump_module_list_stream_data(log, mini_dump, index, options);
        break;
    case MemoryListStream:
        dump_mini_dump_memory_list_stream_data(log, mini_dump, index, options);
        break;
    case ExceptionStream:
        dump_mini_dump_exception_stream_data(log, mini_dump, index, options, symbol_engine);
        break;
    case SystemInfoStream:
        dump_mini_dump_system_info_stream_data(log, mini_dump, index);
        break;
    case ThreadExListStream:
        dump_mini_dump_thread_list_ex_stream_data(log, mini_dump, index, options, symbol_engine);
        break;
    case Memory64ListStream:
        dump_mini_dump_memory64_list_stream_data(log, mini_dump, index, options);
        break;
    case CommentStreamA:
        dump_mini_dump_comment_a_stream_data(log, mini_dump, index);
        break;
    case CommentStreamW:
        dump_mini_dump_comment_w_stream_data(log, mini_dump, index);
        break;
    case HandleDataStream:
        dump_mini_dump_handle_data_stream_data(log, mini_dump, index, options);
        break;
    case FunctionTableStream:
        dump_mini_dump_function_table_stream_data(log, mini_dump, index, options);
        break;
    case UnloadedModuleListStream:
        dump_mini_dump_unloaded_module_list_stream_data(log, mini_dump, index);
        break;
    case MiscInfoStream:
        dump_mini_dump_misc_info_stream_data(log, mini_dump, index);
        break;
    case MemoryInfoListStream:
        dump_mini_dump_memory_info_list_stream_data(log, mini_dump, index);
        break;
    case ThreadInfoListStream:
        dump_mini_dump_thread_info_list_stream_data(log, mini_dump, index);
        break;
    case HandleOperationListStream:
        dump_mini_dump_handle_operation_list_stream_data(log, mini_dump, index, options, symbol_engine);
        break;
    case TokenStream:
        dump_mini_dump_token_stream_data(log, mini_dump, index, options);
        break;
    case SystemMemoryInfoStream:
        dump_mini_dump_system_memory_info_stream_data(log, mini_dump, index);
        break;
    case ProcessVmCountersStream:
        dump_mini_dump_process_vm_counters_stream_data(log, mini_dump, index);
        break;
    case IptTraceStream:
        dump_mini_dump_ipt_trace_stream_data(log, mini_dump, index);
        break;
    case ThreadNamesStream:
        dump_mini_dump_thread_names_stream_data(log, mini_dump, index);
        break;

    default:  // NOLINT(clang-diagnostic-covered-switch-default)
        hex_dump_stream = true;
        if (type < LastReservedStream)
        {
            log << L"Unknown Stream Type\n";
        }
        break;
    }

    if (options.hex_dump_stream_data() || hex_dump_stream)
    {
        log << L"Data:\n";
        hex_dump::hex_dump(log, mini_dump.rva32(entry.Location), entry.Location.DataSize, 1);
        log << L'\n';
    }
}
