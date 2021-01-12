#pragma once
#include "../DbgHelpUtils/mini_dump.h"
#include "../DbgHelpUtils/process_vm_counters_stream.h"

namespace MiniDumpExplorerApp
{
    struct DumpFileStreamProcessVmCounters : winrt::implements<DumpFileStreamProcessVmCounters, winrt::MiniDumpExplorer::IDumpFileStreamProcessVmCounters>
    {
        DumpFileStreamProcessVmCounters(size_t index, dlg_help_utils::mini_dump const& mini_dump);

        [[nodiscard]] uint64_t Index() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] uint64_t ProcessVmCountersVersion() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpProcessVmCounters1 ProcessVmCounters1() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpProcessVmCounters2 ProcessVmCounters2() const;

    private:
        dlg_help_utils::process_vm_counters_stream vm_counters_;
        winrt::MiniDumpExplorer::IMiniDumpProcessVmCounters1 vm_counters_1_;
        winrt::MiniDumpExplorer::IMiniDumpProcessVmCounters2 vm_counters_2_;
    };
}

