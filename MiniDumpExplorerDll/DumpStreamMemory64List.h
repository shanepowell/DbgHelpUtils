#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamMemory64List.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamMemory64List : DumpStreamMemory64ListT<DumpStreamMemory64List>
    {
        DumpStreamMemory64List();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamMemory64List : DumpStreamMemory64ListT<DumpStreamMemory64List, implementation::DumpStreamMemory64List>
    {
    };
}
