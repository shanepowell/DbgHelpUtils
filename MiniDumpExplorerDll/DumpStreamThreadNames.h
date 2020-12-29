#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamThreadNames.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamThreadNames : DumpStreamThreadNamesT<DumpStreamThreadNames>
    {
        DumpStreamThreadNames();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamThreadNames : DumpStreamThreadNamesT<DumpStreamThreadNames, implementation::DumpStreamThreadNames>
    {
    };
}
