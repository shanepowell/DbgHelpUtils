#include "mini_dump_stream_type.h"

#include <format>
#include <unordered_map>

#include "stream_hex_dump.h"

using namespace std::string_literals;
using namespace dlg_help_utils::stream_hex_dump;

namespace
{
    using namespace dlg_help_utils::mini_dump_stream_type;

    std::unordered_map<MINIDUMP_STREAM_TYPE, std::tuple<std::wstring, std::wstring>> const g_type_map =  // NOLINT(clang-diagnostic-exit-time-destructors, cppcoreguidelines-interfaces-global-init)
    {
        {UnusedStream, {L"Unused"s, enum_names::UnusedStream}},
        {ReservedStream0, {L"Reserved 0"s, enum_names::ReservedStream0}},
        {ReservedStream1, {L"Reserved 1"s, enum_names::ReservedStream1}},
        {ThreadListStream, {L"Thread List"s, enum_names::ThreadListStream}},
        {ModuleListStream, {L"Module List"s, enum_names::ModuleListStream}},
        {MemoryListStream, {L"Memory List"s, enum_names::MemoryListStream}},
        {ExceptionStream, {L"Exception"s, enum_names::ExceptionStream}},
        {SystemInfoStream, {L"System Information"s, enum_names::SystemInfoStream}},
        {ThreadExListStream, {L"Thread Extension List"s, enum_names::ThreadExListStream}},
        {Memory64ListStream, {L"Memory64 List"s, enum_names::Memory64ListStream}},
        {CommentStreamA, {L"Comment Ascii"s, enum_names::CommentStreamA}},
        {CommentStreamW, {L"Comment Unicode"s, enum_names::CommentStreamW}},
        {HandleDataStream, {L"Handle Data"s, enum_names::HandleDataStream}},
        {FunctionTableStream, {L"Function Table"s, enum_names::FunctionTableStream}},
        {UnloadedModuleListStream, {L"Unloaded Module List"s, enum_names::UnloadedModuleListStream}},
        {MiscInfoStream, {L"Miscellaneous Information"s, enum_names::MiscInfoStream}},
        {MemoryInfoListStream, {L"Memory Information List"s, enum_names::MemoryInfoListStream}},
        {ThreadInfoListStream, {L"Thread Information List"s, enum_names::ThreadInfoListStream}},
        {HandleOperationListStream, {L"Handle Operation List"s, enum_names::HandleOperationListStream}},
        {TokenStream, {L"Token"s, enum_names::TokenStream}},
        {JavaScriptDataStream, {L"Java Script Data"s, enum_names::JavaScriptDataStream}},
        {SystemMemoryInfoStream, {L"System Memory Information"s, enum_names::SystemMemoryInfoStream}},
        {ProcessVmCountersStream, {L"Process VM Counters"s, enum_names::ProcessVmCountersStream}},
        {IptTraceStream, {L"IPT Trace"s, enum_names::IptTraceStream}},
        {ThreadNamesStream, {L"Thread Names"s, enum_names::ThreadNamesStream}},
        {ceStreamNull, {L"CE Null"s, enum_names::ceStreamNull}},
        {ceStreamSystemInfo, {L"CE System Information"s, enum_names::ceStreamSystemInfo}},
        {ceStreamException, {L"CE Exception"s, enum_names::ceStreamException}},
        {ceStreamModuleList, {L"CE Module List"s, enum_names::ceStreamModuleList}},
        {ceStreamProcessList, {L"CE Process List"s, enum_names::ceStreamProcessList}},
        {ceStreamThreadList, {L"CE Thread List"s, enum_names::ceStreamThreadList}},
        {ceStreamThreadContextList, {L"CE Thread Context List"s, enum_names::ceStreamThreadContextList}},
        {ceStreamThreadCallStackList, {L"CE Thread Call Stack List"s, enum_names::ceStreamThreadCallStackList}},
        {ceStreamMemoryVirtualList, {L"CE Memory Virtual List"s, enum_names::ceStreamMemoryVirtualList}},
        {ceStreamMemoryPhysicalList, {L"CE Memory Physical List"s, enum_names::ceStreamMemoryPhysicalList}},
        {ceStreamBucketParameters, {L"CE Bucket Parameters"s, enum_names::ceStreamBucketParameters}},
        {ceStreamProcessModuleMap, {L"CE Process Module Map"s, enum_names::ceStreamProcessModuleMap}},
        {ceStreamDiagnosisList, {L"CE Diagnosis List"s, enum_names::ceStreamDiagnosisList}},
        {LastReservedStream, {L"Last Reserved Stream"s, enum_names::LastReservedStream}},
    };
}

namespace dlg_help_utils::mini_dump_stream_type
{
    std::wstring to_wstring(MINIDUMP_STREAM_TYPE const type)
    {
        if (type > LastReservedStream)
        {
            return std::format(L"User Stream Type [{}]", to_hex(type));
        }

        auto const it = g_type_map.find(type);
        if (it == g_type_map.end())
        {
            return std::format(L"Unknown Stream Type [{}]", to_hex(type));
        }

        return std::get<0>(it->second);
    }

    std::wstring to_enum_wstring(MINIDUMP_STREAM_TYPE const type)
    {
        if (type > LastReservedStream)
        {
            return to_hex(type);
        }

        auto const it = g_type_map.find(type);
        if (it == g_type_map.end())
        {
            return std::format(L"unknown stream type [{}]", to_hex(type));
        }

        return std::get<1>(it->second);
    }
}

