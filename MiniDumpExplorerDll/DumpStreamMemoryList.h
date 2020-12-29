#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamMemoryList.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamMemoryList : DumpStreamMemoryListT<DumpStreamMemoryList>
    {
        DumpStreamMemoryList();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamMemoryList : DumpStreamMemoryListT<DumpStreamMemoryList, implementation::DumpStreamMemoryList>
    {
    };
}
