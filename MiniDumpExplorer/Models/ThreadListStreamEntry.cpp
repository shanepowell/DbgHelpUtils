#include "pch.h"
#include "ThreadListStreamEntry.h"

#include "MiniDumpMemoryDescriptor.h"
#include "DbgHelpUtils/exit_scope.h"
#include "DbgHelpUtils/mini_dump_memory_walker_release_handle.h"
#include "DbgHelpUtils/thread_info_utils.h"
#include "Helpers/SymbolEngineHelper.h"
#include "Helpers/WindowHelper.h"
#include "Models/ThreadStackEntry.h"
#include "Utility/logger.h"

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
            },
            {
            }, 
            {
            },
            {
            })
    {
    }

    void ThreadListStreamEntry::Set(uint32_t const index, dlg_help_utils::stream_thread thread)
    {
        index_ = index;
        thread_ = std::move(thread);
        name_ = thread_.thread_name();
        priorityClassType_ = dlg_help_utils::thread_info_utils::thread_priority_class_to_string(thread_->PriorityClass);
        stack_.as<MiniDumpMemoryDescriptor>()->Set(thread_->Stack);
    }

    fire_and_forget ThreadListStreamEntry::LoadStack(dlg_help_utils::mini_dump const& mini_dump)
    {
        if(stackLoading_)
        {
            co_return;
        }

        stackLoading_ = true;

        // ReSharper disable once CppTooWideScope
        apartment_context ui_thread;

        stackEntries_.Clear();

        auto anchor_self = get_strong();
        auto weak_self = get_weak();
        auto& symbolEngineHelper = SymbolEngineHelper::Instance();

        co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());

        auto walker_store = std::make_shared<Utility::mini_dump_walker_store>(mini_dump, thread_, symbolEngineHelper.symbol_engine());
        auto const threadId = thread_->ThreadId;
        {
            dlg_help_utils::scope_exit release_handle{[&walker_store]{ walker_store->walker_.release_handle(); } };
            logger::Log().LogMessage(log_level::debug, std::format("LoadStack start for thread [{}]", threadId));

            for(auto stackCallEntry : dump_stack(walker_store->walker_, symbolEngineHelper.symbol_engine(), thread_.thread_context(), 0))
            {
                // ReSharper disable once CppAssignedValueIsNeverUsed
                anchor_self = {};
                if(WindowHelper::IsExiting())
                {
                    co_return;
                }

                MiniDumpExplorer::ThreadStackEntry entry;
                entry.as<ThreadStackEntry>()->Set(walker_store, stackCallEntry);

                if(WindowHelper::IsExiting())
                {
                    co_return;
                }

                dlg_help_utils::stream_stack_dump::mini_dump_memory_walker_release_handle handle{walker_store->walker_};
                {
                    logger::Log().LogMessage(log_level::debug, std::format("Loading UI thread stack entry [{}] for thread [{}]", stackCallEntry.index, threadId));
                    co_await ui_thread;

                    anchor_self = weak_self.get();
                    if(anchor_self && !WindowHelper::IsExiting())
                    {
                        stackEntries_.Append(entry);
                    }
                    else
                    {
                        // it's been removed while loading the items
                        // need to switch QueueController thread to avoid symbol_engine callback fault
                        co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());
                        co_return;
                    }
                    co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());
                    logger::Log().LogMessage(log_level::debug, std::format("Completed loading UI thread stack entry [{}] for thread [{}]", stackCallEntry.index, threadId));
                }
            }
        }

        logger::Log().LogMessage(log_level::debug, std::format("LoadStack end for thread [{}]", threadId));
    }
}
