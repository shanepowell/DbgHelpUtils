#include "pch.h"
#include "ThreadStack.h"

#include "MiniDumpMemoryDescriptor.h"
#include "DbgHelpUtils/exit_scope.h"
#include "DbgHelpUtils/mini_dump_memory_walker_release_handle.h"
#include "DbgHelpUtils/thread_info_utils.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Helpers/SymbolEngineHelper.h"
#include "Helpers/WindowHelper.h"
#include "Models/ThreadStackEntry.h"
#include "Utility/logger.h"
#include "Utility/run.h"

#if __has_include("ThreadStack.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadStack.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadStack::ThreadStack()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
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

    void ThreadStack::Set(dlg_help_utils::stream_thread thread)
    {
        thread_ = std::move(thread);
        stackMemoryRange_.as<MiniDumpMemoryDescriptor>()->Set(thread_->Stack);
    }

    void ThreadStack::Set(dlg_help_utils::stream_thread_ex thread)
    {
        threadEx_ = std::move(thread);
    }

    void ThreadStack::LoadStack(dlg_help_utils::mini_dump const& mini_dump)
    {
        if(stackLoading_)
        {
            return;
        }

        stackLoading_ = true;

        if (HasStackMemoryRange())
        {
            LoadThreadStack(mini_dump);
        }
        else
        {
            LoadThreadExStack(mini_dump);
        }
    }

    fire_and_forget ThreadStack::LoadThreadStack(dlg_help_utils::mini_dump const& mini_dump)
    {
        auto const threadId = thread_->ThreadId;
        co_await Utility::run(__FUNCTION__, [this, &mini_dump, threadId]()->Windows::Foundation::IAsyncAction
            {
                // ReSharper disable once CppTooWideScope
                apartment_context ui_thread;

                stackEntries_.Clear();

                auto anchor_self = get_strong();
                auto weak_self = get_weak();
                auto& symbolEngineHelper = SymbolEngineHelper::Instance();

                co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());

                {
                    auto walker_store = std::make_shared<Utility::mini_dump_walker_store>(mini_dump, thread_, symbolEngineHelper.symbol_engine());
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

                logger::Log().LogMessage(log_level::debug, std::format("LoadThreadStack end for thread [{}]", threadId));
            }, [threadId] { return Utility::for_thread_id(threadId); });
    }

    fire_and_forget ThreadStack::LoadThreadExStack(dlg_help_utils::mini_dump const& mini_dump)
    {
        auto const threadId = threadEx_->ThreadId;
        co_await Utility::run(__FUNCTION__, [this, &mini_dump, threadId]()->Windows::Foundation::IAsyncAction
            {
                // ReSharper disable once CppTooWideScope
                apartment_context ui_thread;

                stackEntries_.Clear();

                auto anchor_self = get_strong();
                auto weak_self = get_weak();
                auto& symbolEngineHelper = SymbolEngineHelper::Instance();

                co_await resume_foreground(symbolEngineHelper.QueueController().DispatcherQueue());

                if (threadEx_.stack() != nullptr)
                {
                    {
                        auto walker_store = std::make_shared<Utility::mini_dump_walker_store>(mini_dump, threadEx_, symbolEngineHelper.symbol_engine());
                        dlg_help_utils::scope_exit release_handle{[&walker_store]{ walker_store->walker_.release_handle(); } };
                        logger::Log().LogMessage(log_level::debug, std::format("LoadStack start for thread [{}]", threadId));

                        for(auto stackCallEntry : dump_stack(walker_store->walker_, symbolEngineHelper.symbol_engine(), threadEx_.thread_context(), 0))
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
                                logger::Log().LogMessage(log_level::debug, std::format("Loading UI threadEx stack entry [{}] for thread [{}]", stackCallEntry.index, threadId));
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
                                logger::Log().LogMessage(log_level::debug, std::format("Completed loading UI threadEx stack entry [{}] for thread [{}]", stackCallEntry.index, threadId));
                            }
                        }
                    }
                }
                logger::Log().LogMessage(log_level::debug, std::format("LoadThreadExStack end for thread [{}]", threadId));
            }, [threadId] { return Utility::for_thread_id(threadId); });
    }
}
