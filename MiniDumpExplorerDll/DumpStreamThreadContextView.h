#pragma once

#include <winrt/impl/MiniDumpExplorer.1.h>
#include "DumpStreamThreadContextView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamThreadContextView : DumpStreamThreadContextViewT<DumpStreamThreadContextView>
    {
        DumpStreamThreadContextView();

        IDumpFileStreamThreadContext ThreadContext() const;
        void ThreadContext(IDumpFileStreamThreadContext value);

    private:
        IDumpFileStreamThreadContext thread_context_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamThreadContextView : DumpStreamThreadContextViewT<DumpStreamThreadContextView, implementation::DumpStreamThreadContextView>
    {
    };
}
