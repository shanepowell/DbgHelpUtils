#pragma once

#include "ProcessVmCountersStreamPage.g.h"

#include "DbgHelpUtils/process_vm_counters_stream.h"
#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct ProcessVmCountersStreamPage : ProcessVmCountersStreamPageT<ProcessVmCountersStreamPage>, GlobalOptionsNotifyPropertyChangedBase<ProcessVmCountersStreamPage>, MiniDumpPageBase<ProcessVmCountersStreamPage>
    {
        ProcessVmCountersStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        uint32_t Index() const { return static_cast<uint32_t>(vm_counters_stream_.index()); }
        uint16_t Revision() const;

        uint16_t HasFlags() const { return vm_counters_stream_.process_vm_counters_version() == 2; }
        uint16_t Flags() const { return vm_counters_stream_.process_vm_counters_2().Flags; }
        Windows::Foundation::Collections::IObservableVector<hstring> FlagsList() const { return flagsList_; }

        bool HasVmCounters() const { return vm_counters_stream_.process_vm_counters_version() == 1 || (vm_counters_stream_.process_vm_counters_2().Flags & MINIDUMP_PROCESS_VM_COUNTERS) == MINIDUMP_PROCESS_VM_COUNTERS; }
        uint32_t PageFaultCount() const;
        uint64_t PeakWorkingSetSize() const;
        uint64_t WorkingSetSize() const;
        uint64_t QuotaPeakPagedPoolUsage() const;
        uint64_t QuotaPagedPoolUsage() const;
        uint64_t QuotaPeakNonPagedPoolUsage() const;
        uint64_t QuotaNonPagedPoolUsage() const;
        uint64_t PagefileUsage() const;
        uint64_t PeakPagefileUsage() const;

        bool HasVirtualSize() const { return vm_counters_stream_.process_vm_counters_version() == 2 && (vm_counters_stream_.process_vm_counters_2().Flags & MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE) == MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE; }
        uint64_t PeakVirtualSize() const { return vm_counters_stream_.process_vm_counters_2().PeakVirtualSize; }
        uint64_t VirtualSize() const { return vm_counters_stream_.process_vm_counters_2().VirtualSize; }
        
        bool HasPrivateUsage() const { return vm_counters_stream_.process_vm_counters_version() == 1 || (vm_counters_stream_.process_vm_counters_2().Flags & MINIDUMP_PROCESS_VM_COUNTERS_EX) == MINIDUMP_PROCESS_VM_COUNTERS_EX; }
        uint64_t PrivateUsage() const;
        
        bool HasVmCountersEx2() const { return vm_counters_stream_.process_vm_counters_version() == 2 && (vm_counters_stream_.process_vm_counters_2().Flags & MINIDUMP_PROCESS_VM_COUNTERS_EX2) == MINIDUMP_PROCESS_VM_COUNTERS_EX2; }
        uint64_t PrivateWorkingSetSize() const { return vm_counters_stream_.process_vm_counters_2().PrivateWorkingSetSize; }
        uint64_t SharedCommitUsage() const { return vm_counters_stream_.process_vm_counters_2().SharedCommitUsage; }

        bool HasJob() const { return vm_counters_stream_.process_vm_counters_version() == 2 && (vm_counters_stream_.process_vm_counters_2().Flags & MINIDUMP_PROCESS_VM_COUNTERS_JOB) == MINIDUMP_PROCESS_VM_COUNTERS_JOB; }
        uint64_t JobSharedCommitUsage() const { return vm_counters_stream_.process_vm_counters_2().JobSharedCommitUsage; }
        uint64_t JobPrivateCommitUsage() const { return vm_counters_stream_.process_vm_counters_2().JobPrivateCommitUsage; }
        uint64_t JobPeakPrivateCommitUsage() const { return vm_counters_stream_.process_vm_counters_2().JobPeakPrivateCommitUsage; }
        uint64_t JobPrivateCommitLimit() const { return vm_counters_stream_.process_vm_counters_2().JobPrivateCommitLimit; }
        uint64_t JobTotalCommitLimit() const { return vm_counters_stream_.process_vm_counters_2().JobTotalCommitLimit; }


    private:
        void SetupFlyoutMenus();
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        dlg_help_utils::process_vm_counters_stream vm_counters_stream_{};
        Windows::Foundation::Collections::IObservableVector<hstring> flagsList_{single_threaded_observable_vector<hstring>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ProcessVmCountersStreamPage : ProcessVmCountersStreamPageT<ProcessVmCountersStreamPage, implementation::ProcessVmCountersStreamPage>
    {
    };
}
