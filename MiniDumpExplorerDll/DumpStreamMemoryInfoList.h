#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamMemoryInfoList.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamMemoryInfoList : DumpStreamMemoryInfoListT<DumpStreamMemoryInfoList>
    {
        DumpStreamMemoryInfoList();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamMemoryInfoList : DumpStreamMemoryInfoListT<DumpStreamMemoryInfoList, implementation::DumpStreamMemoryInfoList>
    {
    };
}
