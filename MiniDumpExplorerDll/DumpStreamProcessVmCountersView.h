#pragma once

#include "DumpStreamProcessVmCountersView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamProcessVmCountersView : DumpStreamProcessVmCountersViewT<DumpStreamProcessVmCountersView>
    {
        DumpStreamProcessVmCountersView(IDumpFileStreamProcessVmCounters stream);

        [[nodiscard]] IDumpFileStreamProcessVmCounters Stream() const;

    private:
        IDumpFileStreamProcessVmCounters stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamProcessVmCountersView : DumpStreamProcessVmCountersViewT<DumpStreamProcessVmCountersView, implementation::DumpStreamProcessVmCountersView>
    {
    };
}
