#pragma once

#include "DumpStreamIptTraceView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamIptTraceView : DumpStreamIptTraceViewT<DumpStreamIptTraceView>
    {
        DumpStreamIptTraceView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamIptTraceView : DumpStreamIptTraceViewT<DumpStreamIptTraceView, implementation::DumpStreamIptTraceView>
    {
    };
}
