#pragma once

#include "DumpStreamHandleDataView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamHandleDataView : DumpStreamHandleDataViewT<DumpStreamHandleDataView>
    {
        DumpStreamHandleDataView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamHandleDataView : DumpStreamHandleDataViewT<DumpStreamHandleDataView, implementation::DumpStreamHandleDataView>
    {
    };
}
