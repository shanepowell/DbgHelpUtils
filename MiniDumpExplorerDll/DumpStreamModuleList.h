#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamModuleList.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamModuleList : DumpStreamModuleListT<DumpStreamModuleList>
    {
        DumpStreamModuleList();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamModuleList : DumpStreamModuleListT<DumpStreamModuleList, implementation::DumpStreamModuleList>
    {
    };
}
