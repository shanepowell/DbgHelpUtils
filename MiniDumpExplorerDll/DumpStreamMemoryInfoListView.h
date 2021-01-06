#pragma once

#include "DumpStreamMemoryInfoListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamMemoryInfoListView : DumpStreamMemoryInfoListViewT<DumpStreamMemoryInfoListView>
    {
        DumpStreamMemoryInfoListView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamMemoryInfoListView : DumpStreamMemoryInfoListViewT<DumpStreamMemoryInfoListView, implementation::DumpStreamMemoryInfoListView>
    {
    };
}
