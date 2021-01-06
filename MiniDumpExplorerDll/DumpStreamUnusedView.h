#pragma once

#include "DumpStreamUnusedView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamUnusedView : DumpStreamUnusedViewT<DumpStreamUnusedView>
    {
        DumpStreamUnusedView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamUnusedView : DumpStreamUnusedViewT<DumpStreamUnusedView, implementation::DumpStreamUnusedView>
    {
    };
}
