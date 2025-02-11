#pragma once

#include "ThreadListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "Utility/mini_dump_walker_store.h"

namespace dlg_help_utils::dbg_help
{
    class symbol_engine;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpMemoryDescriptor;
    struct ThreadStackEntry;
    struct ThreadContext;

    struct ThreadListStreamEntry : ThreadListStreamEntryT<ThreadListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<ThreadListStreamEntry>
    {
        ThreadListStreamEntry();

        void Set(uint32_t index, dlg_help_utils::stream_thread thread);
        fire_and_forget LoadStack(dlg_help_utils::mini_dump const& mini_dump);

        uint32_t Index() const { return index_; }
        uint32_t Id() const { return thread_->ThreadId; }
        hstring Name() const { return name_; }
        uint32_t Priority() const { return thread_->Priority; }
        uint32_t PriorityClass() const { return thread_->PriorityClass; }
        hstring PriorityClassType() const { return priorityClassType_; }
        uint32_t SuspendCount() const { return thread_->SuspendCount; }
        uint64_t TEB() const { return thread_->Teb; }
        MiniDumpExplorer::MiniDumpMemoryDescriptor Stack() const { return stack_; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::ThreadStackEntry> StackEntries() const { return stackEntries_; }
        MiniDumpExplorer::ThreadContext ThreadContext() const { return threadContext_; }

    private:
        uint32_t index_{};
        dlg_help_utils::stream_thread thread_{};
        hstring name_;
        hstring priorityClassType_;
        MiniDumpExplorer::MiniDumpMemoryDescriptor stack_;
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::ThreadStackEntry> stackEntries_{single_threaded_observable_vector<MiniDumpExplorer::ThreadStackEntry>()};
        bool stackLoading_{false};
        MiniDumpExplorer::ThreadContext threadContext_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadListStreamEntry : ThreadListStreamEntryT<ThreadListStreamEntry, implementation::ThreadListStreamEntry>
    {
    };
}
