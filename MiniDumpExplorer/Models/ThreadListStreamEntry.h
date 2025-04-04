#pragma once

#include "ThreadListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "Utility/mini_dump_walker_store.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadStack;
    struct ThreadContext;

    struct ThreadListStreamEntry : ThreadListStreamEntryT<ThreadListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<ThreadListStreamEntry>
    {
        ThreadListStreamEntry();

        void Set(uint32_t index, dlg_help_utils::stream_thread thread);
        void LoadStack(dlg_help_utils::mini_dump const& mini_dump) const;

        uint32_t Index() const { return index_; }
        uint32_t Id() const { return thread_->ThreadId; }
        hstring Name() const { return name_; }
        uint32_t Priority() const { return thread_->Priority; }
        uint32_t PriorityClass() const { return thread_->PriorityClass; }
        hstring PriorityClassType() const { return priorityClassType_; }
        uint32_t SuspendCount() const { return thread_->SuspendCount; }
        uint64_t TEB() const { return thread_->Teb; }
        MiniDumpExplorer::ThreadStack Stack() const { return stack_; }
        MiniDumpExplorer::ThreadContext ThreadContext() const { return threadContext_; }

    private:
        uint32_t index_{};
        dlg_help_utils::stream_thread thread_{};
        hstring name_;
        hstring priorityClassType_;
        MiniDumpExplorer::ThreadStack stack_;
        MiniDumpExplorer::ThreadContext threadContext_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadListStreamEntry : ThreadListStreamEntryT<ThreadListStreamEntry, implementation::ThreadListStreamEntry>
    {
    };
}
