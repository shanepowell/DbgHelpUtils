#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamIptTrace.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamIptTrace : DumpStreamIptTraceT<DumpStreamIptTrace>
    {
        DumpStreamIptTrace();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamIptTrace : DumpStreamIptTraceT<DumpStreamIptTrace, implementation::DumpStreamIptTrace>
    {
    };
}
