#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamUnused.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamUnused : DumpStreamUnusedT<DumpStreamUnused>
    {
        DumpStreamUnused();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamUnused : DumpStreamUnusedT<DumpStreamUnused, implementation::DumpStreamUnused>
    {
    };
}
