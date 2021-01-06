#pragma once

#include <winrt/impl/MiniDumpExplorer.1.h>
#include "DumpStreamX64ThreadContextView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamX64ThreadContextView : DumpStreamX64ThreadContextViewT<DumpStreamX64ThreadContextView>
    {
        DumpStreamX64ThreadContextView();

        IDumpFileStreamX64ThreadContext ThreadContext() const;
        void ThreadContext(IDumpFileStreamX64ThreadContext value);

    private:
        IDumpFileStreamX64ThreadContext thread_context_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamX64ThreadContextView : DumpStreamX64ThreadContextViewT<DumpStreamX64ThreadContextView, implementation::DumpStreamX64ThreadContextView>
    {
    };
}
