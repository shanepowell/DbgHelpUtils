#pragma once

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
