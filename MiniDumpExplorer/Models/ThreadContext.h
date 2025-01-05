#pragma once

#include "ThreadContext.g.h"

#include "DbgHelpUtils/stream_thread_context.h"
#include "NotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct X64ThreadContext;

    struct ThreadContext : ThreadContextT<ThreadContext>, NotifyPropertyChangedBase<ThreadContext>
    {
        ThreadContext();

        void Set(dlg_help_utils::stream_thread_context const& thread_context);

        bool HasX64ThreadContext() const { return x64ThreadContext_ != nullptr; }
        MiniDumpExplorer::X64ThreadContext X64ThreadContext() const { return x64ThreadContext_; }

        bool HasX86ThreadContext() const { return x64ThreadContext_ != nullptr; }
        bool HasWow64ThreadContext() const { return x64ThreadContext_ != nullptr; }

    private:
        MiniDumpExplorer::X64ThreadContext x64ThreadContext_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadContext : ThreadContextT<ThreadContext, implementation::ThreadContext>
    {
    };
}
