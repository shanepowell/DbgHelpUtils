#include "pch.h"
#include "DbgHelpUtils/mini_dump_stream_type.h"

#include <format>
#include <unordered_map>

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/stream_hex_dump.h"

using namespace std::string_literals;
using namespace dlg_help_utils::stream_hex_dump;

namespace
{
    using namespace dlg_help_utils::mini_dump_stream_type;

    std::unordered_map<MINIDUMP_STREAM_TYPE, std::wstring> const type_map =  // NOLINT(cppcoreguidelines-interfaces-global-init)
    {
        {UnusedStream, enum_names::UnusedStream},
        {ReservedStream0, enum_names::ReservedStream0},
        {ReservedStream1, enum_names::ReservedStream1},
        {ThreadListStream, enum_names::ThreadListStream},
        {ModuleListStream, enum_names::ModuleListStream},
        {MemoryListStream, enum_names::MemoryListStream},
        {ExceptionStream, enum_names::ExceptionStream},
        {SystemInfoStream, enum_names::SystemInfoStream},
        {ThreadExListStream, enum_names::ThreadExListStream},
        {Memory64ListStream, enum_names::Memory64ListStream},
        {CommentStreamA, enum_names::CommentStreamA},
        {CommentStreamW, enum_names::CommentStreamW},
        {HandleDataStream, enum_names::HandleDataStream},
        {FunctionTableStream, enum_names::FunctionTableStream},
        {UnloadedModuleListStream, enum_names::UnloadedModuleListStream},
        {MiscInfoStream, enum_names::MiscInfoStream},
        {MemoryInfoListStream, enum_names::MemoryInfoListStream},
        {ThreadInfoListStream, enum_names::ThreadInfoListStream},
        {HandleOperationListStream, enum_names::HandleOperationListStream},
        {TokenStream, enum_names::TokenStream},
        {JavaScriptDataStream, enum_names::JavaScriptDataStream},
        {SystemMemoryInfoStream, enum_names::SystemMemoryInfoStream},
        {ProcessVmCountersStream, enum_names::ProcessVmCountersStream},
        {IptTraceStream, enum_names::IptTraceStream},
        {ThreadNamesStream, enum_names::ThreadNamesStream},
        {ceStreamNull, enum_names::ceStreamNull},
        {ceStreamSystemInfo, enum_names::ceStreamSystemInfo},
        {ceStreamException, enum_names::ceStreamException},
        {ceStreamModuleList, enum_names::ceStreamModuleList},
        {ceStreamProcessList, enum_names::ceStreamProcessList},
        {ceStreamThreadList, enum_names::ceStreamThreadList},
        {ceStreamThreadContextList, enum_names::ceStreamThreadContextList},
        {ceStreamThreadCallStackList, enum_names::ceStreamThreadCallStackList},
        {ceStreamMemoryVirtualList, enum_names::ceStreamMemoryVirtualList},
        {ceStreamMemoryPhysicalList, enum_names::ceStreamMemoryPhysicalList},
        {ceStreamBucketParameters, enum_names::ceStreamBucketParameters},
        {ceStreamProcessModuleMap, enum_names::ceStreamProcessModuleMap},
        {ceStreamDiagnosisList, enum_names::ceStreamDiagnosisList},
        {LastReservedStream, enum_names::LastReservedStream},
    };
}

namespace dlg_help_utils::mini_dump_stream_type
{
    std::wstring to_wstring(MINIDUMP_STREAM_TYPE const type)
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        if (type > LastReservedStream)
        {
            return std::format(L"{} [{}]", rm.MainResourceMap().GetValue(L"Resources/UserStreamType").ValueAsString(), to_hex(type));
        }

        auto const it = type_map.find(type);
        if (it == type_map.end())
        {
            return std::format(L"{} [{}]", rm.MainResourceMap().GetValue(L"Resources/UnknownStreamType").ValueAsString(), to_hex(type));
        }

        return std::wstring{rm.MainResourceMap().GetValue(std::format(L"Resources/{}",it->second)).ValueAsString()};
    }

    std::wstring to_enum_wstring(MINIDUMP_STREAM_TYPE const type)
    {
        if (type > LastReservedStream)
        {
            return to_hex(type);
        }

        auto const it = type_map.find(type);
        if (it == type_map.end())
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            return std::format(L"{} [{}]", rm.MainResourceMap().GetValue(L"Resources/UnknownStreamType").ValueAsString(), to_hex(type));
        }

        return it->second;
    }
}

