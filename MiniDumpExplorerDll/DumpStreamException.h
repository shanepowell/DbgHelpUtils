#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamException.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamException : DumpStreamExceptionT<DumpStreamException>
    {
        DumpStreamException();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamException : DumpStreamExceptionT<DumpStreamException, implementation::DumpStreamException>
    {
    };
}
