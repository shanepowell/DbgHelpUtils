#include "pch.h"
#include "ThreadExListStreamEntry.h"

#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/thread_info_utils.h"
#include "Models/MiniDumpMemoryDescriptor.h"
#include "Models/ThreadStack.h"
#include "Models/ThreadContext.h"

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
        threadContext_.as<implementation::ThreadContext>()->Set(thread_.thread_context());
        stack_.as<ThreadStack>()->Set(thread_);

        if (thread.backing_store() != nullptr)
        {
            backingStore_.as<MiniDumpMemoryDescriptor>()->Set(thread->BackingStore);
            std::wstringstream ss;
            dlg_help_utils::hex_dump::hex_dump(ss, thread.backing_store(), thread.backing_store_size(), 1);
            backingStoreHexDump_ = std::move(ss).str();
        }
    }

    void ThreadExListStreamEntry::LoadStack(dlg_help_utils::mini_dump const& mini_dump) const
    {
        stack_.as<ThreadStack>()->LoadStack(mini_dump);
    }
}
