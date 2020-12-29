#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamHandleData.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamHandleData : DumpStreamHandleDataT<DumpStreamHandleData>
    {
        DumpStreamHandleData();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamHandleData : DumpStreamHandleDataT<DumpStreamHandleData, implementation::DumpStreamHandleData>
    {
    };
}
