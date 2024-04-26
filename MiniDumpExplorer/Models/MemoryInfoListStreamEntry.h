#pragma once

#include "MemoryInfoListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"
#include <DbgHelp.h>

namespace winrt::MiniDumpExplorer::implementation
{
    struct MemoryInfoListStreamEntry : MemoryInfoListStreamEntryT<MemoryInfoListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<MemoryInfoListStreamEntry>
    {
        MemoryInfoListStreamEntry();

        uint32_t Index() const { return index_; }
        uint64_t BaseAddress() const { return memory_info_->BaseAddress; }
        uint64_t AllocationBase() const { return memory_info_->AllocationBase; }
        uint32_t AllocationProtect() const { return memory_info_->AllocationProtect; }
        Windows::Foundation::Collections::IObservableVector<hstring> AllocationProtectList() const { return allocationProtectList_; }
        hstring AllocationProtectListString() const { return allocationProtectListString_; }
        uint64_t RegionSize() const { return memory_info_->RegionSize; }
        uint32_t State() const { return memory_info_->State; }
        hstring StateString() const { return stateString_; }
        uint32_t Protect() const { return memory_info_->Protect; }
        Windows::Foundation::Collections::IObservableVector<hstring> ProtectList() const { return protectList_; }
        hstring ProtectListString() const { return protectListString_; }
        uint32_t Type() const { return memory_info_->Type; }
        hstring TypeString() const { return typeString_; }

        void Set(uint32_t index, MINIDUMP_MEMORY_INFO const* memory_info);

    private:
        static void fill_protect_option_list(uint32_t protect_options, hstring& options_string, Windows::Foundation::Collections::IObservableVector<hstring> const& options_list);

    private:
        uint32_t index_{};
        MINIDUMP_MEMORY_INFO const* memory_info_{};
        hstring stateString_;
        hstring allocationProtectListString_;
        hstring protectListString_;
        hstring typeString_;
        Windows::Foundation::Collections::IObservableVector<hstring> allocationProtectList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<hstring> protectList_{single_threaded_observable_vector<hstring>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MemoryInfoListStreamEntry : MemoryInfoListStreamEntryT<MemoryInfoListStreamEntry, implementation::MemoryInfoListStreamEntry>
    {
    };
}
