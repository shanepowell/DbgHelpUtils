#pragma once

#include <winrt/impl/MiniDumpExplorer.1.h>
#include "DumpStreamThreadContext.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamThreadContext : DumpStreamThreadContextT<DumpStreamThreadContext>
    {
        DumpStreamThreadContext();

        IDumpFileStreamThreadContext ThreadContext() const;
        void ThreadContext(IDumpFileStreamThreadContext value);

    private:
        IDumpFileStreamThreadContext thread_context_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamThreadContext : DumpStreamThreadContextT<DumpStreamThreadContext, implementation::DumpStreamThreadContext>
    {
    };
}
