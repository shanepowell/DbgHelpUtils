#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <string>

namespace dlg_help_utils::mini_dump_stream_type
{
    namespace enum_names
    {
        extern wchar_t const* const UnusedStream;
        extern wchar_t const* const ReservedStream0;
        extern wchar_t const* const ReservedStream1;
        extern wchar_t const* const ThreadListStream;
        extern wchar_t const* const ModuleListStream;
        extern wchar_t const* const MemoryListStream;
        extern wchar_t const* const ExceptionStream;
        extern wchar_t const* const SystemInfoStream;
        extern wchar_t const* const ThreadExListStream;
        extern wchar_t const* const Memory64ListStream;
        extern wchar_t const* const CommentStreamA;
        extern wchar_t const* const CommentStreamW;
        extern wchar_t const* const HandleDataStream;
        extern wchar_t const* const FunctionTableStream;
        extern wchar_t const* const UnloadedModuleListStream;
        extern wchar_t const* const MiscInfoStream;
        extern wchar_t const* const MemoryInfoListStream;
        extern wchar_t const* const ThreadInfoListStream;
        extern wchar_t const* const HandleOperationListStream;
        extern wchar_t const* const TokenStream;
        extern wchar_t const* const JavaScriptDataStream;
        extern wchar_t const* const SystemMemoryInfoStream;
        extern wchar_t const* const ProcessVmCountersStream;
        extern wchar_t const* const IptTraceStream;
        extern wchar_t const* const ThreadNamesStream;
        extern wchar_t const* const ceStreamNull;
        extern wchar_t const* const ceStreamSystemInfo;
        extern wchar_t const* const ceStreamException;
        extern wchar_t const* const ceStreamModuleList;
        extern wchar_t const* const ceStreamProcessList;
        extern wchar_t const* const ceStreamThreadList;
        extern wchar_t const* const ceStreamThreadContextList;
        extern wchar_t const* const ceStreamThreadCallStackList;
        extern wchar_t const* const ceStreamMemoryVirtualList;
        extern wchar_t const* const ceStreamMemoryPhysicalList;
        extern wchar_t const* const ceStreamBucketParameters;
        extern wchar_t const* const ceStreamProcessModuleMap;
        extern wchar_t const* const ceStreamDiagnosisList;
        extern wchar_t const* const LastReservedStream;
    }

    std::wstring to_wstring(MINIDUMP_STREAM_TYPE type);
    std::wstring to_enum_wstring(MINIDUMP_STREAM_TYPE type);
    MINIDUMP_STREAM_TYPE from_wstring(std::wstring const& type);
}
