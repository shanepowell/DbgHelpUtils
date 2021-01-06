#pragma once

#include "DumpStreamThreadExListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamThreadExListView : DumpStreamThreadExListViewT<DumpStreamThreadExListView>
    {
        DumpStreamThreadExListView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamThreadExListView : DumpStreamThreadExListViewT<DumpStreamThreadExListView, implementation::DumpStreamThreadExListView>
    {
    };
}
