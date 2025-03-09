#pragma once

#include "X86ThreadContextView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct X86ThreadContext;

    struct X86ThreadContextView : X86ThreadContextViewT<X86ThreadContextView>
    {
        X86ThreadContextView();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        MiniDumpExplorer::X86ThreadContext ThreadContext() const { return threadContext_; }
        void ThreadContext(MiniDumpExplorer::X86ThreadContext const& value) { threadContext_ = value; }

    private:
        void SetupFlyoutMenus();

    private:
        MiniDumpExplorer::X86ThreadContext threadContext_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct X86ThreadContextView : X86ThreadContextViewT<X86ThreadContextView, implementation::X86ThreadContextView>
    {
    };
}
