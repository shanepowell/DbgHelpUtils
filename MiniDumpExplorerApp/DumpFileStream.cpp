#include "pch.h"
#include "DumpFileStream.h"

#include "../DbgHelpUtils/mini_dump_stream_type.h"
#include "GetResult.h"
#include "DumpFileStreamComment.h"
#include "DumpFileStreamException.h"
#include "DumpFileStreamMiscInfo.h"
#include "DumpFileStreamSystemInfo.h"
#include "DumpFileStreamProcessVmCounters.h"

namespace MiniDumpExplorerApp
{

    DumpFileStream::DumpFileStream(size_t index, MINIDUMP_DIRECTORY const& directory_entry, dlg_help_utils::mini_dump const& mini_dump, DbgHelpDispatcher& dispatcher, MiniDumpData& mini_data_data, dlg_help_utils::dbg_help::symbol_engine& symbol_engine)
    : index_{index}
    , directory_entry_{directory_entry}
    , mini_dump_{mini_dump}
    , dispatcher_{dispatcher}
    , mini_data_data_{mini_data_data}
    , symbol_engine_{symbol_engine}
    , type_{dlg_help_utils::mini_dump_stream_type::to_string(static_cast<MINIDUMP_STREAM_TYPE>(directory_entry_.StreamType))}
    {
        switch(static_cast<MINIDUMP_STREAM_TYPE>(directory_entry_.StreamType))
        {
        case CommentStreamA:
            comment_ = *winrt::make_self<DumpFileStreamComment>(index_, mini_dump_, CommentType::Ascii);
            break;

        case CommentStreamW:
            comment_ =  *winrt::make_self<DumpFileStreamComment>(index_, mini_dump_, CommentType::Unicode);
            break;

        case ExceptionStream:
            exception_ = *winrt::make_self<DumpFileStreamException>(index_, mini_dump_, dispatcher_, mini_data_data_, symbol_engine_);
            break;

        case SystemInfoStream:
            system_info_ = *winrt::make_self<DumpFileStreamSystemInfo>(index_, mini_dump_);
            break;

        case MiscInfoStream:
            misc_info_ = *winrt::make_self<DumpFileStreamMiscInfo>(index_, mini_dump_);
            break;

        case ProcessVmCountersStream:
            process_vm_counters_ = *winrt::make_self<DumpFileStreamProcessVmCounters>(index_, mini_dump_);
            break;

        case UnusedStream:
        case ReservedStream0:
        case ReservedStream1:
        case ThreadListStream:
        case ModuleListStream:
        case MemoryListStream:
        case ThreadExListStream:
        case Memory64ListStream:
        case HandleDataStream:
        case FunctionTableStream:
        case UnloadedModuleListStream:
        case MemoryInfoListStream:
        case ThreadInfoListStream:
        case HandleOperationListStream:
        case TokenStream:
        case JavaScriptDataStream:
        case SystemMemoryInfoStream:
        case IptTraceStream:
        case ThreadNamesStream:
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
        default:
            break;
        }
    }

    uint64_t DumpFileStream::Index() const
    {
        return index_;
    }

    winrt::MiniDumpExplorer::StreamType DumpFileStream::Type() const
    {
        return static_cast<winrt::MiniDumpExplorer::StreamType>(directory_entry_.StreamType);
    }

    winrt::hstring DumpFileStream::TypeString() const
    {
        return type_;
    }

    winrt::MiniDumpExplorer::IDumpFileStreamComment DumpFileStream::AsComment() const
    {
        return GetResult(comment_);
    }

    winrt::MiniDumpExplorer::IDumpFileStreamException DumpFileStream::AsException() const
    {
        return GetResult(exception_);
    }

    winrt::MiniDumpExplorer::IDumpFileStreamSystemInfo DumpFileStream::AsSystemInfo() const
    {
        return GetResult(system_info_);
    }

    winrt::MiniDumpExplorer::IDumpFileStreamMiscInfo DumpFileStream::AsMiscInfo() const
    {
        return GetResult(misc_info_);
    }

    winrt::MiniDumpExplorer::IDumpFileStreamProcessVmCounters DumpFileStream::AsProcessVmCounters() const
    {
        return GetResult(process_vm_counters_);
    }
}
