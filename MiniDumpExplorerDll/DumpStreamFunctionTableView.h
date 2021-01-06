#pragma once

#include "DumpStreamFunctionTableView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamFunctionTableView : DumpStreamFunctionTableViewT<DumpStreamFunctionTableView>
    {
        DumpStreamFunctionTableView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamFunctionTableView : DumpStreamFunctionTableViewT<DumpStreamFunctionTableView, implementation::DumpStreamFunctionTableView>
    {
    };
}
