#include "mini_dump_stream_type.h"
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "stream_hex_dump.h"
#include "wide_runtime_error.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;

namespace
{
    std::unordered_map<MINIDUMP_STREAM_TYPE, std::tuple<std::wstring, std::wstring>> const type_map =
    {
        {UnusedStream, {L"Unused"s, L"UnusedStream"s}},
        {ReservedStream0, {L"Reserved 0"s, L"ReservedStream0"s}},
        {ReservedStream1, {L"Reserved 1"s, L"ReservedStream1"s}},
        {ThreadListStream, {L"Thread List"s, L"ThreadListStream"s}},
        {ModuleListStream, {L"Module List"s, L"ModuleListStream"s}},
        {MemoryListStream, {L"Memory List"s, L"MemoryListStream"s}},
        {ExceptionStream, {L"Exception"s, L"ExceptionStream"s}},
        {SystemInfoStream, {L"System Info"s, L"SystemInfoStream"s}},
        {ThreadExListStream, {L"ThreadEx List"s, L"ThreadExListStream"s}},
        {Memory64ListStream, {L"MemoryEx List"s, L"Memory64ListStream"s}},
        {CommentStreamA, {L"Comment Ascii"s, L"CommentStreamA"s}},
        {CommentStreamW, {L"Comment Unicode"s, L"CommentStreamW"s}},
        {HandleDataStream, {L"Handle Data"s, L"HandleDataStream"s}},
        {FunctionTableStream, {L"Function Table"s, L"FunctionTableStream"s}},
        {UnloadedModuleListStream, {L"Unloaded Module List"s, L"UnloadedModuleListStream"s}},
        {MiscInfoStream, {L"Misc Info"s, L"MiscInfoStream"s}},
        {MemoryInfoListStream, {L"Memory Info List"s, L"MemoryInfoListStream"s}},
        {ThreadInfoListStream, {L"Thread Info List"s, L"ThreadInfoListStream"s}},
        {HandleOperationListStream, {L"Handle Operation List"s, L"HandleOperationListStream"s}},
        {TokenStream, {L"Token"s, L"TokenStream"s}},
        {JavaScriptDataStream, {L"Java Script Data"s, L"JavaScriptDataStream"s}},
        {SystemMemoryInfoStream, {L"System Memory Info"s, L"SystemMemoryInfoStream"s}},
        {ProcessVmCountersStream, {L"Process Vm Counters"s, L"ProcessVmCountersStream"s}},
        {IptTraceStream, {L"IPT Trace"s, L"IptTraceStream"s}},
        {ThreadNamesStream, {L"Thread Names"s, L"ThreadNamesStream"s}},
        {ceStreamNull, {L"CE Null"s, L"ceStreamNull"s}},
        {ceStreamSystemInfo, {L"CE System Info"s, L"ceStreamSystemInfo"s}},
        {ceStreamException, {L"CE Exception"s, L"ceStreamException"s}},
        {ceStreamModuleList, {L"CE Module List"s, L"ceStreamModuleList"s}},
        {ceStreamProcessList, {L"CE Process List"s, L"ceStreamProcessList"s}},
        {ceStreamThreadList, {L"CE Thread List"s, L"ceStreamThreadList"s}},
        {ceStreamThreadContextList, {L"CE Thread Context List"s, L"ceStreamThreadContextList"s}},
        {ceStreamThreadCallStackList, {L"CE Thread Call Stack List"s, L"ceStreamThreadCallStackList"s}},
        {ceStreamMemoryVirtualList, {L"CE Memory Virtual List"s, L"ceStreamMemoryVirtualList"s}},
        {ceStreamMemoryPhysicalList, {L"CE Memory Physical List"s, L"ceStreamMemoryPhysicalList"s}},
        {ceStreamBucketParameters, {L"CE Bucket Parameters"s, L"ceStreamBucketParameters"s}},
        {ceStreamProcessModuleMap, {L"CE Process Module Map"s, L"ceStreamProcessModuleMap"s}},
        {ceStreamDiagnosisList, {L"CE Diagnosis List"s, L"ceStreamDiagnosisList"s}},
        {LastReservedStream, {L"Last Reserved Stream"s, L"LastReservedStream"s}},
    };
}

namespace dlg_help_utils::mini_dump_stream_type
{
    wstring to_string(MINIDUMP_STREAM_TYPE const type)
    {
        if (type > LastReservedStream)
        {
            return (wostringstream{} << L"User Stream Type [" << to_hex(type) << L"]").str();
        }

        auto const it = type_map.find(type);
        if (it == type_map.end())
        {
            return (wostringstream{} << L"Unknown Stream Type [" << to_hex(type) << L"]").str();
        }

        return std::get<0>(it->second);
    }

    wstring to_enum_string(MINIDUMP_STREAM_TYPE const type)
    {
        if (type > LastReservedStream)
        {
            return (wostringstream{} << to_hex(type)).str();
        }

        auto const it = type_map.find(type);
        if (it == type_map.end())
        {
            return (wostringstream{} << L"unknown stream type [" << to_hex(type) << L"]").str();
        }

        return std::get<1>(it->second);
    }

    MINIDUMP_STREAM_TYPE from_string(std::wstring const& type)
    {
        if (!type.empty() && isdigit(type[0]))
        {
            try
            {
                size_t pos;
                auto const type_number = std::stoul(type, &pos, 0);
                if (pos == type.size())
                {
                    return static_cast<MINIDUMP_STREAM_TYPE>(type_number);
                }
            }
            catch (std::invalid_argument const&)
            {
            }
            catch (std::out_of_range const&)
            {
            }
        }

        for (size_t i = 0; i <= LastReservedStream; ++i)
        {
            if (auto const enum_type = static_cast<MINIDUMP_STREAM_TYPE>(i); boost::iequals(type, to_string(enum_type)) || boost::iequals(type, to_enum_string(enum_type)))
            {
                return enum_type;
            }
        }

        // ReSharper disable once StringLiteralTypo
        throw exceptions::wide_runtime_error{
            (wostringstream{} << "invalid MINIDUMP_STREAM_TYPE : [" << type << "]").str()
        };
    }
}
