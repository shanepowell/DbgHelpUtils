#include "pch.h"
#include "ThreadInfoListStreamEntry.h"

#include "DbgHelpUtils/thread_info_utils.h"
#include "DumpFileTimeStamp.h"

#if __has_include("ThreadInfoListStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadInfoListStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadInfoListStreamEntry::ThreadInfoListStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"DumpFlags",
                L"DumpError",
                L"ExitStatus",
                L"StartAddress",
                L"Affinity",
            },
            {
            },
            {
                L"CreateTime",
                L"ExitTime",
            },
            {
                L"KernelTime",
                L"UserTime",
            },
            {
            },
            {
            })
    {
    }

    void ThreadInfoListStreamEntry::Set(uint32_t const index, dlg_help_utils::stream_thread_info thread, dlg_help_utils::time_utils::locale_timezone_info const& dump_file_timezone_info)
    {
        index_ = index;
        thread_ = std::move(thread);
        name_ = thread_.thread_name();
        dumpFlagsListString_ = dlg_help_utils::thread_info_utils::dump_flags_to_string(thread_->DumpFlags);

        dumpFlagsList_.Clear();
        for (auto const& dump_flag : dlg_help_utils::thread_info_utils::dump_flags_to_strings(thread_->DumpFlags))
        {
            dumpFlagsList_.Append(dump_flag);
        }

        if(thread_->CreateTime > 0)
        {
            createTime_.as<DumpFileTimeStamp>()->Set(dlg_help_utils::time_utils::to_filetime(thread->CreateTime), dump_file_timezone_info);
        }

        if(thread_->ExitTime > 0)
        {
            createTime_.as<DumpFileTimeStamp>()->Set(dlg_help_utils::time_utils::to_filetime(thread->ExitTime), dump_file_timezone_info);
        }
    }
}
