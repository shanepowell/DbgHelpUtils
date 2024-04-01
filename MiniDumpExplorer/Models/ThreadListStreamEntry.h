#pragma once

#include "ThreadListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_thread.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadListStreamEntry : ThreadListStreamEntryT<ThreadListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<ThreadListStreamEntry>
    {
        ThreadListStreamEntry();

        void Set(uint32_t index, dlg_help_utils::stream_thread thread);

        uint32_t Index() const { return index_; }
        uint32_t Id() const { return thread_->ThreadId; }
        hstring Name() const { return name_; }
        uint32_t Priority() const { return thread_->Priority; }
        uint32_t PriorityClass() const { return thread_->PriorityClass; }
        hstring PriorityClassType() const { return priorityClassType_; }
        uint32_t SuspendCount() const { return thread_->SuspendCount; }
        uint64_t TEB() const { return thread_->Teb; }

    private:
        uint32_t index_{};
        dlg_help_utils::stream_thread thread_{};
        hstring name_;
        hstring priorityClassType_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadListStreamEntry : ThreadListStreamEntryT<ThreadListStreamEntry, implementation::ThreadListStreamEntry>
    {
    };
}
