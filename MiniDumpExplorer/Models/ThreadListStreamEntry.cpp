#include "pch.h"
#include "ThreadListStreamEntry.h"

#include "DbgHelpUtils/thread_info_utils.h"

#if __has_include("ThreadListStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadListStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadListStreamEntry::ThreadListStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Id",
                L"Priority",
                L"PriorityClass",
                L"TEB",
            }, { })
    {
    }

    void ThreadListStreamEntry::Set(uint32_t const index, dlg_help_utils::stream_thread thread)
    {
        index_ = index;
        thread_ = std::move(thread);
        name_ = thread_.thread_name();
        priorityClassType_ = dlg_help_utils::thread_info_utils::thread_priority_class_to_string(thread_->PriorityClass);
    }
}
