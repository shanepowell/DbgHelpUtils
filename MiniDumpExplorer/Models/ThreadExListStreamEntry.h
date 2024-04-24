#pragma once

#include "ThreadExListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_thread_ex.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadExListStreamEntry : ThreadExListStreamEntryT<ThreadExListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<ThreadExListStreamEntry>
    {
        ThreadExListStreamEntry();

        void Set(uint32_t index, dlg_help_utils::stream_thread_ex thread);

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
        dlg_help_utils::stream_thread_ex thread_{};
        hstring name_;
        hstring priorityClassType_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadExListStreamEntry : ThreadExListStreamEntryT<ThreadExListStreamEntry, implementation::ThreadExListStreamEntry>
    {
    };
}
