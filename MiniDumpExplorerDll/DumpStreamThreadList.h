#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamThreadList.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamThreadList : DumpStreamThreadListT<DumpStreamThreadList>
    {
        DumpStreamThreadList();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamThreadList : DumpStreamThreadListT<DumpStreamThreadList, implementation::DumpStreamThreadList>
    {
    };
}
