#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamThreadExList.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamThreadExList : DumpStreamThreadExListT<DumpStreamThreadExList>
    {
        DumpStreamThreadExList();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamThreadExList : DumpStreamThreadExListT<DumpStreamThreadExList, implementation::DumpStreamThreadExList>
    {
    };
}
