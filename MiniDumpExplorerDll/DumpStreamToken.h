#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamToken.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamToken : DumpStreamTokenT<DumpStreamToken>
    {
        DumpStreamToken();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamToken : DumpStreamTokenT<DumpStreamToken, implementation::DumpStreamToken>
    {
    };
}
