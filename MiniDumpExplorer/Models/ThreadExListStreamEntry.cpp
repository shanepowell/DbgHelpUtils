#include "pch.h"
#include "ThreadExListStreamEntry.h"

#include "DbgHelpUtils/thread_info_utils.h"

#if __has_include("ThreadExListStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadExListStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadExListStreamEntry::ThreadExListStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Id",
                L"Priority",
                L"PriorityClass",
                L"TEB",
            },
            {
            }, 
            {
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void ThreadExListStreamEntry::Set(uint32_t const index, dlg_help_utils::stream_thread_ex thread)
    {
        index_ = index;
        thread_ = std::move(thread);
        name_ = thread_.thread_name();
        priorityClassType_ = dlg_help_utils::thread_info_utils::thread_priority_class_to_string(thread_->PriorityClass);
    }
}
