#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DumpStreamComment.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamComment : DumpStreamCommentT<DumpStreamComment>
    {
        DumpStreamComment();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamComment : DumpStreamCommentT<DumpStreamComment, implementation::DumpStreamComment>
    {
    };
}
