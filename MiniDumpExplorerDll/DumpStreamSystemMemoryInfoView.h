#pragma once

#include "DumpStreamSystemMemoryInfoView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamSystemMemoryInfoView : DumpStreamSystemMemoryInfoViewT<DumpStreamSystemMemoryInfoView>
    {
        DumpStreamSystemMemoryInfoView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamSystemMemoryInfoView : DumpStreamSystemMemoryInfoViewT<DumpStreamSystemMemoryInfoView, implementation::DumpStreamSystemMemoryInfoView>
    {
    };
}
