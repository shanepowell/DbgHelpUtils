#pragma once

#include <winrt/impl/MiniDumpExplorer.1.h>
#include "DumpStreamX86ThreadContextView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamX86ThreadContextView : DumpStreamX86ThreadContextViewT<DumpStreamX86ThreadContextView>
    {
        DumpStreamX86ThreadContextView();

        IDumpFileStreamX86ThreadContext ThreadContext() const;
        void ThreadContext(IDumpFileStreamX86ThreadContext value);

        bool HasExtendedRegisters() const;
        void HasExtendedRegisters(bool value);

    private:
        IDumpFileStreamX86ThreadContext thread_context_;
        bool has_extended_registers_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamX86ThreadContextView : DumpStreamX86ThreadContextViewT<DumpStreamX86ThreadContextView, implementation::DumpStreamX86ThreadContextView>
    {
    };
}
