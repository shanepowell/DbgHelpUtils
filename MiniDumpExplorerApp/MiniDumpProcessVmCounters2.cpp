#include "pch.h"
#include "MiniDumpProcessVmCounters2.h"

#include "../DbgHelpUtils/process_vm_counters_stream.h"
#include "../DbgHelpUtils/system_info_utils.h"

namespace MiniDumpExplorerApp
{
    MiniDumpProcessVmCounters2::MiniDumpProcessVmCounters2(dlg_help_utils::process_vm_counters_stream const& vm_counters)
        : vm_counters_{vm_counters}
    {
        for (auto const& flag_type : dlg_help_utils::system_info_utils::vm_counters_2_flags_to_strings(Flags()))
        {
            flag_types_.Append(winrt::hstring{flag_type});
        }
    }

    uint16_t MiniDumpProcessVmCounters2::Revision() const
    {
        return vm_counters_.process_vm_counters_2().Revision;
    }

    uint16_t MiniDumpProcessVmCounters2::Flags() const
    {
        return vm_counters_.process_vm_counters_2().Flags;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> MiniDumpProcessVmCounters2::FlagTypes() const
    {
        return flag_types_;
    }

    bool MiniDumpProcessVmCounters2::HasProcessVmCounters() const
    {
        return (Flags() & MINIDUMP_PROCESS_VM_COUNTERS) == MINIDUMP_PROCESS_VM_COUNTERS;
    }

    uint32_t MiniDumpProcessVmCounters2::PageFaultCount() const
    {
        return vm_counters_.process_vm_counters_2().PageFaultCount;
    }

    uint64_t MiniDumpProcessVmCounters2::PeakWorkingSetSize() const
    {
        return vm_counters_.process_vm_counters_2().PeakWorkingSetSize;
    }

    uint64_t MiniDumpProcessVmCounters2::WorkingSetSize() const
    {
        return vm_counters_.process_vm_counters_2().WorkingSetSize;
    }

    uint64_t MiniDumpProcessVmCounters2::QuotaPeakPagedPoolUsage() const
    {
        return vm_counters_.process_vm_counters_2().QuotaPeakPagedPoolUsage;
    }

    uint64_t MiniDumpProcessVmCounters2::QuotaPagedPoolUsage() const
    {
        return vm_counters_.process_vm_counters_2().QuotaPagedPoolUsage;
    }

    uint64_t MiniDumpProcessVmCounters2::QuotaPeakNonPagedPoolUsage() const
    {
        return vm_counters_.process_vm_counters_2().QuotaPeakNonPagedPoolUsage;
    }

    uint64_t MiniDumpProcessVmCounters2::QuotaNonPagedPoolUsage() const
    {
        return vm_counters_.process_vm_counters_2().QuotaNonPagedPoolUsage;
    }

    uint64_t MiniDumpProcessVmCounters2::PagefileUsage() const
    {
        return vm_counters_.process_vm_counters_2().PagefileUsage;
    }

    uint64_t MiniDumpProcessVmCounters2::PeakPagefileUsage() const
    {
        return vm_counters_.process_vm_counters_2().PeakPagefileUsage;
    }

    bool MiniDumpProcessVmCounters2::HasProcessVmCountersVirtualSize() const
    {
        return (Flags() & MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE) == MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE;
    }

    uint64_t MiniDumpProcessVmCounters2::PeakVirtualSize() const
    {
        return vm_counters_.process_vm_counters_2().PeakVirtualSize;
    }

    uint64_t MiniDumpProcessVmCounters2::VirtualSize() const
    {
        return vm_counters_.process_vm_counters_2().VirtualSize;
    }

    bool MiniDumpProcessVmCounters2::HasProcessVmCountersPrivateUsage() const
    {
        return (Flags() & MINIDUMP_PROCESS_VM_COUNTERS_EX) == MINIDUMP_PROCESS_VM_COUNTERS_EX;
    }

    uint64_t MiniDumpProcessVmCounters2::PrivateUsage() const
    {
        return vm_counters_.process_vm_counters_2().PrivateUsage;
    }

    bool MiniDumpProcessVmCounters2::HasProcessVmCountersEx2() const
    {
        return (Flags() & MINIDUMP_PROCESS_VM_COUNTERS_EX2) == MINIDUMP_PROCESS_VM_COUNTERS_EX2;
    }

    uint64_t MiniDumpProcessVmCounters2::PrivateWorkingSetSize() const
    {
        return vm_counters_.process_vm_counters_2().PrivateWorkingSetSize;
    }

    uint64_t MiniDumpProcessVmCounters2::SharedCommitUsage() const
    {
        return vm_counters_.process_vm_counters_2().SharedCommitUsage;
    }

    bool MiniDumpProcessVmCounters2::HasProcessVmCountersJob() const
    {
        return (Flags() & MINIDUMP_PROCESS_VM_COUNTERS_JOB) == MINIDUMP_PROCESS_VM_COUNTERS_JOB;
    }

    uint64_t MiniDumpProcessVmCounters2::JobSharedCommitUsage() const
    {
        return vm_counters_.process_vm_counters_2().JobSharedCommitUsage;
    }

    uint64_t MiniDumpProcessVmCounters2::JobPrivateCommitUsage() const
    {
        return vm_counters_.process_vm_counters_2().JobPrivateCommitUsage;
    }

    uint64_t MiniDumpProcessVmCounters2::JobPeakPrivateCommitUsage() const
    {
        return vm_counters_.process_vm_counters_2().JobPeakPrivateCommitUsage;
    }

    uint64_t MiniDumpProcessVmCounters2::JobPrivateCommitLimit() const
    {
        return vm_counters_.process_vm_counters_2().JobPrivateCommitLimit;
    }

    uint64_t MiniDumpProcessVmCounters2::JobTotalCommitLimit() const
    {
        return vm_counters_.process_vm_counters_2().JobTotalCommitLimit;
    }
}
