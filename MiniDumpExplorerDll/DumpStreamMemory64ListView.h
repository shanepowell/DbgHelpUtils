#pragma once

#include "DumpStreamMemory64ListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamMemory64ListView : DumpStreamMemory64ListViewT<DumpStreamMemory64ListView>
    {
        DumpStreamMemory64ListView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamMemory64ListView : DumpStreamMemory64ListViewT<DumpStreamMemory64ListView, implementation::DumpStreamMemory64ListView>
    {
    };
}
