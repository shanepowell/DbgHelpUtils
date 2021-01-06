#pragma once
#include "DumpStreamHandleOperationListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamHandleOperationListView : DumpStreamHandleOperationListViewT<DumpStreamHandleOperationListView>
    {
        DumpStreamHandleOperationListView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamHandleOperationListView : DumpStreamHandleOperationListViewT<DumpStreamHandleOperationListView, implementation::DumpStreamHandleOperationListView>
    {
    };
}
