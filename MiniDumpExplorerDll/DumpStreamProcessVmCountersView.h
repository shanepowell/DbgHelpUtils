#pragma once

#include "DumpStreamProcessVmCountersView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamProcessVmCountersView : DumpStreamProcessVmCountersViewT<DumpStreamProcessVmCountersView>
    {
        DumpStreamProcessVmCountersView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamProcessVmCountersView : DumpStreamProcessVmCountersViewT<DumpStreamProcessVmCountersView, implementation::DumpStreamProcessVmCountersView>
    {
    };
}
