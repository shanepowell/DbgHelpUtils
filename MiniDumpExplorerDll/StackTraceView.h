#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "StackTraceView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct StackTraceView : StackTraceViewT<StackTraceView>
    {
        StackTraceView();

        IStackTrace StackTrace() const;
        void StackTrace(IStackTrace value);

    private:
        IStackTrace stack_trace_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct StackTraceView : StackTraceViewT<StackTraceView, implementation::StackTraceView>
    {
    };
}
