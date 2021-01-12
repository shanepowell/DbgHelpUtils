#pragma once

namespace dlg_help_utils
{
    class process_vm_counters_stream;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpProcessVmCounters2 : winrt::implements<MiniDumpProcessVmCounters2, winrt::MiniDumpExplorer::IMiniDumpProcessVmCounters2>
    {
        MiniDumpProcessVmCounters2(dlg_help_utils::process_vm_counters_stream const& vm_counters);

        [[nodiscard]] uint16_t Revision() const;
        [[nodiscard]] uint16_t Flags() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> FlagTypes() const;
        [[nodiscard]] bool HasProcessVmCounters() const;
        [[nodiscard]] uint32_t PageFaultCount() const;
        [[nodiscard]] uint64_t PeakWorkingSetSize() const;
        [[nodiscard]] uint64_t WorkingSetSize() const;
        [[nodiscard]] uint64_t QuotaPeakPagedPoolUsage() const;
        [[nodiscard]] uint64_t QuotaPagedPoolUsage() const;
        [[nodiscard]] uint64_t QuotaPeakNonPagedPoolUsage() const;
        [[nodiscard]] uint64_t QuotaNonPagedPoolUsage() const;
        [[nodiscard]] uint64_t PagefileUsage() const;
        [[nodiscard]] uint64_t PeakPagefileUsage() const;
        [[nodiscard]] bool HasProcessVmCountersVirtualSize() const;
        [[nodiscard]] uint64_t PeakVirtualSize() const;
        [[nodiscard]] uint64_t VirtualSize() const;
        [[nodiscard]] bool HasProcessVmCountersPrivateUsage() const;
        [[nodiscard]] uint64_t PrivateUsage() const;
        [[nodiscard]] bool HasProcessVmCountersEx2() const;
        [[nodiscard]] uint64_t PrivateWorkingSetSize() const;
        [[nodiscard]] uint64_t SharedCommitUsage() const;
        [[nodiscard]] bool HasProcessVmCountersJob() const;
        [[nodiscard]] uint64_t JobSharedCommitUsage() const;
        [[nodiscard]] uint64_t JobPrivateCommitUsage() const;
        [[nodiscard]] uint64_t JobPeakPrivateCommitUsage() const;
        [[nodiscard]] uint64_t JobPrivateCommitLimit() const;
        [[nodiscard]] uint64_t JobTotalCommitLimit() const;

    private:
        dlg_help_utils::process_vm_counters_stream const& vm_counters_;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> flag_types_{winrt::single_threaded_observable_vector<winrt::hstring>()};
    };
}
