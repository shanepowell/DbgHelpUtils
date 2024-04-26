#pragma once

#include "ThreadInfoListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_thread_info.h"

namespace dlg_help_utils::time_utils
{
    struct locale_timezone_info;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpFileTimeStamp;

    struct ThreadInfoListStreamEntry : ThreadInfoListStreamEntryT<ThreadInfoListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<ThreadInfoListStreamEntry>
    {
        ThreadInfoListStreamEntry();

        void Set(uint32_t index, dlg_help_utils::stream_thread_info thread, dlg_help_utils::time_utils::locale_timezone_info const& dump_file_timezone_info);

        uint32_t Index() const { return index_; }
        uint32_t Id() const { return thread_->ThreadId; }
        hstring Name() const { return name_; }
        uint32_t DumpFlags() const { return thread_->DumpFlags; }
        Windows::Foundation::Collections::IObservableVector<hstring> DumpFlagsList() const { return dumpFlagsList_; }
        hstring DumpFlagsListString() const { return dumpFlagsListString_; }
        uint32_t DumpError() const { return thread_->DumpError; }
        uint32_t ExitStatus() const { return thread_->ExitStatus; }
        MiniDumpExplorer::DumpFileTimeStamp CreateTime() const { return createTime_; }
        MiniDumpExplorer::DumpFileTimeStamp ExitTime() const { return exitTime_; }
        uint64_t KernelTime() const { return thread_->KernelTime; }
        uint64_t UserTime() const { return thread_->UserTime; }
        uint64_t StartAddress() const { return thread_->StartAddress; }
        uint64_t Affinity() const { return thread_->Affinity; }

    private:
        uint32_t index_{};
        dlg_help_utils::stream_thread_info thread_{};
        hstring name_;
        hstring dumpFlagsListString_;
        Windows::Foundation::Collections::IObservableVector<hstring> dumpFlagsList_{single_threaded_observable_vector<hstring>()};
        MiniDumpExplorer::DumpFileTimeStamp createTime_{};
        MiniDumpExplorer::DumpFileTimeStamp exitTime_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadInfoListStreamEntry : ThreadInfoListStreamEntryT<ThreadInfoListStreamEntry, implementation::ThreadInfoListStreamEntry>
    {
    };
}
