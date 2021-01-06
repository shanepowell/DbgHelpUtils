#pragma once

#include "DumpStreamMemoryListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamMemoryListView : DumpStreamMemoryListViewT<DumpStreamMemoryListView>
    {
        DumpStreamMemoryListView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamMemoryListView : DumpStreamMemoryListViewT<DumpStreamMemoryListView, implementation::DumpStreamMemoryListView>
    {
    };
}
