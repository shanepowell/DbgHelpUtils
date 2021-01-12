#include "pch.h"
#include "MiniDumpProcessVmCounters1.h"

#include "../DbgHelpUtils/process_vm_counters_stream.h"

namespace MiniDumpExplorerApp
{
    MiniDumpProcessVmCounters1::MiniDumpProcessVmCounters1(dlg_help_utils::process_vm_counters_stream const& vm_counters)
        : vm_counters_{vm_counters}
    {
    }

    uint16_t MiniDumpProcessVmCounters1::Revision() const
    {
        return vm_counters_.process_vm_counters().Revision;
    }

    uint32_t MiniDumpProcessVmCounters1::PageFaultCount() const
    {
        return vm_counters_.process_vm_counters().PageFaultCount;
    }

    uint64_t MiniDumpProcessVmCounters1::PeakWorkingSetSize() const
    {
        return vm_counters_.process_vm_counters().PeakWorkingSetSize;
    }

    uint64_t MiniDumpProcessVmCounters1::WorkingSetSize() const
    {
        return vm_counters_.process_vm_counters().WorkingSetSize;
    }

    uint64_t MiniDumpProcessVmCounters1::QuotaPeakPagedPoolUsage() const
    {
        return vm_counters_.process_vm_counters().QuotaPeakPagedPoolUsage;
    }

    uint64_t MiniDumpProcessVmCounters1::QuotaPagedPoolUsage() const
    {
        return vm_counters_.process_vm_counters().QuotaPagedPoolUsage;
    }

    uint64_t MiniDumpProcessVmCounters1::QuotaPeakNonPagedPoolUsage() const
    {
        return vm_counters_.process_vm_counters().QuotaPeakNonPagedPoolUsage;
    }

    uint64_t MiniDumpProcessVmCounters1::QuotaNonPagedPoolUsage() const
    {
        return vm_counters_.process_vm_counters().QuotaNonPagedPoolUsage;
    }

    uint64_t MiniDumpProcessVmCounters1::PagefileUsage() const
    {
        return vm_counters_.process_vm_counters().PagefileUsage;
    }

    uint64_t MiniDumpProcessVmCounters1::PeakPagefileUsage() const
    {
        return vm_counters_.process_vm_counters().PeakPagefileUsage;
    }

    uint64_t MiniDumpProcessVmCounters1::PrivateUsage() const
    {
        return vm_counters_.process_vm_counters().PrivateUsage;
    }
}
