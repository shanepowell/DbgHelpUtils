#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamSystemInfo.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamSystemInfo : DumpStreamSystemInfoT<DumpStreamSystemInfo>
    {
        DumpStreamSystemInfo();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamSystemInfo : DumpStreamSystemInfoT<DumpStreamSystemInfo, implementation::DumpStreamSystemInfo>
    {
    };
}
