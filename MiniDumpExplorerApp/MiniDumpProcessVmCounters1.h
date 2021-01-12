#pragma once

namespace dlg_help_utils
{
    class process_vm_counters_stream;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpProcessVmCounters1 : winrt::implements<MiniDumpProcessVmCounters1, winrt::MiniDumpExplorer::IMiniDumpProcessVmCounters1>
    {
        MiniDumpProcessVmCounters1(dlg_help_utils::process_vm_counters_stream const& vm_counters);

        [[nodiscard]] uint16_t Revision() const;
        [[nodiscard]] uint32_t PageFaultCount() const;
        [[nodiscard]] uint64_t PeakWorkingSetSize() const;
        [[nodiscard]] uint64_t WorkingSetSize() const;
        [[nodiscard]] uint64_t QuotaPeakPagedPoolUsage() const;
        [[nodiscard]] uint64_t QuotaPagedPoolUsage() const;
        [[nodiscard]] uint64_t QuotaPeakNonPagedPoolUsage() const;
        [[nodiscard]] uint64_t QuotaNonPagedPoolUsage() const;
        [[nodiscard]] uint64_t PagefileUsage() const;
        [[nodiscard]] uint64_t PeakPagefileUsage() const;
        [[nodiscard]] uint64_t PrivateUsage() const;

    private:
        dlg_help_utils::process_vm_counters_stream const& vm_counters_;
    };
}
