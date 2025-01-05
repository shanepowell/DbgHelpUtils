#pragma once

#include "ThreadContextView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadContext;

    struct ThreadContextView : ThreadContextViewT<ThreadContextView>
    {
        ThreadContextView();

        MiniDumpExplorer::ThreadContext ThreadContext() const { return threadContext_; }
        void ThreadContext(MiniDumpExplorer::ThreadContext const& value) { threadContext_ = value; }

    private:
        MiniDumpExplorer::ThreadContext threadContext_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadContextView : ThreadContextViewT<ThreadContextView, implementation::ThreadContextView>
    {
    };
}
