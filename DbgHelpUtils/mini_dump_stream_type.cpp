#include "mini_dump_stream_type.h"

#include <format>

#include "string_compare.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::mini_dump_stream_type
{
    namespace enum_names
    {
        wchar_t const* const UnusedStream = L"UnusedStream";
        wchar_t const* const ReservedStream0 = L"ReservedStream0";
        wchar_t const* const ReservedStream1 = L"ReservedStream1";
        wchar_t const* const ThreadListStream = L"ThreadListStream";
        wchar_t const* const ModuleListStream = L"ModuleListStream";
        wchar_t const* const MemoryListStream = L"MemoryListStream";
        wchar_t const* const ExceptionStream = L"ExceptionStream";
        wchar_t const* const SystemInfoStream = L"SystemInfoStream";
        wchar_t const* const ThreadExListStream = L"ThreadExListStream";
        wchar_t const* const Memory64ListStream = L"Memory64ListStream";
        wchar_t const* const CommentStreamA = L"CommentStreamA";
        wchar_t const* const CommentStreamW = L"CommentStreamW";
        wchar_t const* const HandleDataStream = L"HandleDataStream";
        wchar_t const* const FunctionTableStream = L"FunctionTableStream";
        wchar_t const* const UnloadedModuleListStream = L"UnloadedModuleListStream";
        wchar_t const* const MiscInfoStream = L"MiscInfoStream";
        wchar_t const* const MemoryInfoListStream = L"MemoryInfoListStream";
        wchar_t const* const ThreadInfoListStream = L"ThreadInfoListStream";
        wchar_t const* const HandleOperationListStream = L"HandleOperationListStream";
        wchar_t const* const TokenStream = L"TokenStream";
        wchar_t const* const JavaScriptDataStream = L"JavaScriptDataStream";
        wchar_t const* const SystemMemoryInfoStream = L"SystemMemoryInfoStream";
        wchar_t const* const ProcessVmCountersStream = L"ProcessVmCountersStream";
        wchar_t const* const IptTraceStream = L"IptTraceStream";
        wchar_t const* const ThreadNamesStream = L"ThreadNamesStream";
        wchar_t const* const ceStreamNull = L"ceStreamNull";
        wchar_t const* const ceStreamSystemInfo = L"ceStreamSystemInfo";
        wchar_t const* const ceStreamException = L"ceStreamException";
        wchar_t const* const ceStreamModuleList = L"ceStreamModuleList";
        wchar_t const* const ceStreamProcessList = L"ceStreamProcessList";
        wchar_t const* const ceStreamThreadList = L"ceStreamThreadList";
        wchar_t const* const ceStreamThreadContextList = L"ceStreamThreadContextList";
        wchar_t const* const ceStreamThreadCallStackList = L"ceStreamThreadCallStackList";
        wchar_t const* const ceStreamMemoryVirtualList = L"ceStreamMemoryVirtualList";
        wchar_t const* const ceStreamMemoryPhysicalList = L"ceStreamMemoryPhysicalList";
        wchar_t const* const ceStreamBucketParameters = L"ceStreamBucketParameters";
        wchar_t const* const ceStreamProcessModuleMap = L"ceStreamProcessModuleMap";
        wchar_t const* const ceStreamDiagnosisList = L"ceStreamDiagnosisList";
        wchar_t const* const LastReservedStream = L"LastReservedStream";
    }

    MINIDUMP_STREAM_TYPE from_wstring(std::wstring const& type)
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
            catch (std::invalid_argument const&)  // NOLINT(bugprone-empty-catch)
            {
                // ignore
            }
            catch (std::out_of_range const&)  // NOLINT(bugprone-empty-catch)
            {
                // ignore
            }
        }

        for (size_t i = 0; i <= LastReservedStream; ++i)
        {
            if (auto const enum_type = static_cast<MINIDUMP_STREAM_TYPE>(i); string_utils::iequals(type, to_wstring(enum_type)) || string_utils::iequals(type, to_enum_wstring(enum_type)))
            {
                return enum_type;
            }
        }

        // ReSharper disable once StringLiteralTypo
        throw exceptions::wide_runtime_error{std::format(L"invalid MINIDUMP_STREAM_TYPE : [{}]", type)};
    }
}
