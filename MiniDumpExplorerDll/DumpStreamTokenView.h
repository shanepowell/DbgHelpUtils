#pragma once

#include "DumpStreamTokenView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamTokenView : DumpStreamTokenViewT<DumpStreamTokenView>
    {
        DumpStreamTokenView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamTokenView : DumpStreamTokenViewT<DumpStreamTokenView, implementation::DumpStreamTokenView>
    {
    };
}
