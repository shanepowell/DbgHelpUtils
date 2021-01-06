#pragma once

#include "DumpStreamThreadInfoListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamThreadInfoListView : DumpStreamThreadInfoListViewT<DumpStreamThreadInfoListView>
    {
        DumpStreamThreadInfoListView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamThreadInfoListView : DumpStreamThreadInfoListViewT<DumpStreamThreadInfoListView, implementation::DumpStreamThreadInfoListView>
    {
    };
}
