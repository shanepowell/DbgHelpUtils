#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamProcessVmCounters.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamProcessVmCounters : DumpStreamProcessVmCountersT<DumpStreamProcessVmCounters>
    {
        DumpStreamProcessVmCounters();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamProcessVmCounters : DumpStreamProcessVmCountersT<DumpStreamProcessVmCounters, implementation::DumpStreamProcessVmCounters>
    {
    };
}
