#pragma once

#include "DumpStreamModuleListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamModuleListView : DumpStreamModuleListViewT<DumpStreamModuleListView>
    {
        DumpStreamModuleListView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamModuleListView : DumpStreamModuleListViewT<DumpStreamModuleListView, implementation::DumpStreamModuleListView>
    {
    };
}
