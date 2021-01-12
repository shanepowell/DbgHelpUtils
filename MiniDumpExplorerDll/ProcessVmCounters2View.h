#pragma once

#include "ProcessVmCounters2View.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ProcessVmCounters2View : ProcessVmCounters2ViewT<ProcessVmCounters2View>
    {
        ProcessVmCounters2View();

        [[nodiscard]] IMiniDumpProcessVmCounters2 VmCounters() const;
        void VmCounters(IMiniDumpProcessVmCounters2 value);

    private:
        IMiniDumpProcessVmCounters2 vm_counters_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ProcessVmCounters2View : ProcessVmCounters2ViewT<ProcessVmCounters2View, implementation::ProcessVmCounters2View>
    {
    };
}
