#pragma once

#include "DumpStreamThreadListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamThreadListView : DumpStreamThreadListViewT<DumpStreamThreadListView>
    {
        DumpStreamThreadListView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamThreadListView : DumpStreamThreadListViewT<DumpStreamThreadListView, implementation::DumpStreamThreadListView>
    {
    };
}
