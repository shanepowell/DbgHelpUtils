#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamHandleOperationList.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamHandleOperationList : DumpStreamHandleOperationListT<DumpStreamHandleOperationList>
    {
        DumpStreamHandleOperationList();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamHandleOperationList : DumpStreamHandleOperationListT<DumpStreamHandleOperationList, implementation::DumpStreamHandleOperationList>
    {
    };
}
