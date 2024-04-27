#pragma once

#include "ThreadNameListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_thread_name.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadNameListStreamEntry : ThreadNameListStreamEntryT<ThreadNameListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<ThreadNameListStreamEntry>
    {
        ThreadNameListStreamEntry();

        void Set(uint32_t index, dlg_help_utils::stream_thread_name thread);

        uint32_t Index() const { return index_; }
        uint32_t Id() const { return thread_->ThreadId; }
        hstring Name() const { return name_; }

    private:
        uint32_t index_{};
        dlg_help_utils::stream_thread_name thread_{};
        hstring name_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadNameListStreamEntry : ThreadNameListStreamEntryT<ThreadNameListStreamEntry, implementation::ThreadNameListStreamEntry>
    {
    };
}
