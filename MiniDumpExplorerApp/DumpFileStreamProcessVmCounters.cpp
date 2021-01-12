#include "pch.h"
#include "DumpFileStreamProcessVmCounters.h"

#include "MiniDumpProcessVmCounters1.h"
#include "MiniDumpProcessVmCounters2.h"

namespace MiniDumpExplorerApp
{
    DumpFileStreamProcessVmCounters::DumpFileStreamProcessVmCounters(size_t const index, dlg_help_utils::mini_dump const& mini_dump)
        : vm_counters_{mini_dump, index}
    {
        if(!IsValid())
        {
            return;
        }

        switch(ProcessVmCountersVersion())
        {
        case 1:
            vm_counters_1_ = *winrt::make_self<MiniDumpProcessVmCounters1>(vm_counters_);
            break;

        case 2:
            vm_counters_2_ = *winrt::make_self<MiniDumpProcessVmCounters2>(vm_counters_);
            break;

        default:
            break;
        }
    }

    uint64_t DumpFileStreamProcessVmCounters::Index() const
    {
        return vm_counters_.index();
    }

    bool DumpFileStreamProcessVmCounters::IsValid() const
    {
        return vm_counters_.is_valid();
    }

    uint64_t DumpFileStreamProcessVmCounters::ProcessVmCountersVersion() const
    {
        return vm_counters_.process_vm_counters_version();
    }

    winrt::MiniDumpExplorer::IMiniDumpProcessVmCounters1 DumpFileStreamProcessVmCounters::ProcessVmCounters1() const
    {
        return vm_counters_1_;
    }

    winrt::MiniDumpExplorer::IMiniDumpProcessVmCounters2 DumpFileStreamProcessVmCounters::ProcessVmCounters2() const
    {
        return vm_counters_2_;
    }
}
