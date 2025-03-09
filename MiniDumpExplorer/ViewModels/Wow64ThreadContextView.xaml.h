#pragma once

#include "Wow64ThreadContextView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct Wow64ThreadContext;

    struct Wow64ThreadContextView : Wow64ThreadContextViewT<Wow64ThreadContextView>
    {
        Wow64ThreadContextView();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        MiniDumpExplorer::Wow64ThreadContext ThreadContext() const { return threadContext_; }
        void ThreadContext(MiniDumpExplorer::Wow64ThreadContext const& value) { threadContext_ = value; }

    private:
        void SetupFlyoutMenus();

    private:
        MiniDumpExplorer::Wow64ThreadContext threadContext_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct Wow64ThreadContextView : Wow64ThreadContextViewT<Wow64ThreadContextView, implementation::Wow64ThreadContextView>
    {
    };
}
