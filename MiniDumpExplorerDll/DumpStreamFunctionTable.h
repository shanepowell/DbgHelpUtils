#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamFunctionTable.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamFunctionTable : DumpStreamFunctionTableT<DumpStreamFunctionTable>
    {
        DumpStreamFunctionTable();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamFunctionTable : DumpStreamFunctionTableT<DumpStreamFunctionTable, implementation::DumpStreamFunctionTable>
    {
    };
}
