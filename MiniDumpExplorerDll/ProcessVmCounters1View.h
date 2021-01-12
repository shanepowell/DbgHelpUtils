#pragma once

#include "ProcessVmCounters1View.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ProcessVmCounters1View : ProcessVmCounters1ViewT<ProcessVmCounters1View>
    {
        ProcessVmCounters1View();

        [[nodiscard]] IMiniDumpProcessVmCounters1 VmCounters() const;
        void VmCounters(IMiniDumpProcessVmCounters1 value);

    private:
        IMiniDumpProcessVmCounters1 vm_counters_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ProcessVmCounters1View : ProcessVmCounters1ViewT<ProcessVmCounters1View, implementation::ProcessVmCounters1View>
    {
    };
}
