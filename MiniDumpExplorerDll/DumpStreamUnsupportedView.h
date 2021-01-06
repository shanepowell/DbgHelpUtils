#pragma once

#include "DumpStreamUnsupportedView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamUnsupportedView : DumpStreamUnsupportedViewT<DumpStreamUnsupportedView>
    {
        DumpStreamUnsupportedView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamUnsupportedView : DumpStreamUnsupportedViewT<DumpStreamUnsupportedView, implementation::DumpStreamUnsupportedView>
    {
    };
}
