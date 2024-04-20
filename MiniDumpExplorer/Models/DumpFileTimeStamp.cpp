#include "pch.h"
#include "DumpFileTimeStamp.h"

#if __has_include("DumpFileTimeStamp.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DumpFileTimeStamp.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpFileTimeStamp::DumpFileTimeStamp() = default;

    void DumpFileTimeStamp::Set(FILETIME const& ft, dlg_help_utils::time_utils::locale_timezone_info const& dump_file_timezone_info)
    {
        fileTime_ = ft;
        systemTime_ = dlg_help_utils::time_utils::filetime_to_system_time(ft);
        dump_file_timezone_info_ = dump_file_timezone_info;
    }

    void DumpFileTimeStamp::Set(SYSTEMTIME const& st, dlg_help_utils::time_utils::locale_timezone_info const& dump_file_timezone_info)
    {
        systemTime_ = st;
        fileTime_ = dlg_help_utils::time_utils::system_time_to_filetime(st);
        dump_file_timezone_info_ = dump_file_timezone_info;
    }

    void DumpFileTimeStamp::Set(time_t const& timeStamp, dlg_help_utils::time_utils::locale_timezone_info const& dump_file_timezone_info)
    {
        timeStamp_ = timeStamp;
        dump_file_timezone_info_ = dump_file_timezone_info;
    }
}
