#pragma once

#include <winrt/impl/MiniDumpExplorer.1.h>
#include "DumpStreamX64ThreadContext.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamX64ThreadContext : DumpStreamX64ThreadContextT<DumpStreamX64ThreadContext>
    {
        DumpStreamX64ThreadContext();

        IDumpFileStreamX64ThreadContext ThreadContext() const;
        void ThreadContext(IDumpFileStreamX64ThreadContext value);

    private:
        IDumpFileStreamX64ThreadContext thread_context_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamX64ThreadContext : DumpStreamX64ThreadContextT<DumpStreamX64ThreadContext, implementation::DumpStreamX64ThreadContext>
    {
    };
}
