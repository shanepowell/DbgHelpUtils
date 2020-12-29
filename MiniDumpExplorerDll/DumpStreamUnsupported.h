#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamUnsupported.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamUnsupported : DumpStreamUnsupportedT<DumpStreamUnsupported>
    {
        DumpStreamUnsupported();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamUnsupported : DumpStreamUnsupportedT<DumpStreamUnsupported, implementation::DumpStreamUnsupported>
    {
    };
}
