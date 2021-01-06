#pragma once

#include "DumpStreamUnloadedModuleListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamUnloadedModuleListView : DumpStreamUnloadedModuleListViewT<DumpStreamUnloadedModuleListView>
    {
        DumpStreamUnloadedModuleListView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamUnloadedModuleListView : DumpStreamUnloadedModuleListViewT<DumpStreamUnloadedModuleListView, implementation::DumpStreamUnloadedModuleListView>
    {
    };
}
