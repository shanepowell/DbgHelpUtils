#pragma once

#include "X64ThreadContextView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct X64ThreadContext;

    struct X64ThreadContextView : X64ThreadContextViewT<X64ThreadContextView>
    {
        X64ThreadContextView();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        MiniDumpExplorer::X64ThreadContext ThreadContext() const { return threadContext_; }
        void ThreadContext(MiniDumpExplorer::X64ThreadContext const& value) { threadContext_ = value; }

    private:
        void SetupFlyoutMenus();

    private:
        MiniDumpExplorer::X64ThreadContext threadContext_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct X64ThreadContextView : X64ThreadContextViewT<X64ThreadContextView, implementation::X64ThreadContextView>
    {
    };
}
