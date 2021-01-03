#pragma once

#include <winrt/impl/MiniDumpExplorer.1.h>
#include "DumpStreamX86ThreadContext.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamX86ThreadContext : DumpStreamX86ThreadContextT<DumpStreamX86ThreadContext>
    {
        DumpStreamX86ThreadContext();

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
    struct DumpStreamX86ThreadContext : DumpStreamX86ThreadContextT<DumpStreamX86ThreadContext, implementation::DumpStreamX86ThreadContext>
    {
    };
}
