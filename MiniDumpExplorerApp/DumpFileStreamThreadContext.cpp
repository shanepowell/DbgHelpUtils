#include "pch.h"
#include "DumpFileStreamThreadContext.h"

#include "GetResult.h"
#include "DumpFileStreamX64ThreadContext.h"
#include "DumpFileStreamX86ThreadContext.h"
#include "../DbgHelpUtils/exception_stream.h"

namespace MiniDumpExplorerApp
{
    DumpFileStreamThreadContext::DumpFileStreamThreadContext(dlg_help_utils::exception_stream const& exception)
        : exception_{exception}
    {
        if(X86ThreadContextAvailable())
        {
            x86_thread_context_ = *winrt::make_self<DumpFileStreamX86ThreadContext>(exception_.thread_context().x86_thread_context());
        }

        if(Wow64ThreadContextAvailable())
        {
            wow64_thread_context_ = *winrt::make_self<DumpFileStreamX86ThreadContext>(exception_.thread_context().wow64_thread_context());
        }

        if(X64ThreadContextAvailable())
        {
            x64_thread_context_ = *winrt::make_self<DumpFileStreamX64ThreadContext>(exception_.thread_context().x64_thread_context());
        }
    }

    bool DumpFileStreamThreadContext::X86ThreadContextAvailable() const
    {
        return exception_.thread_context().x86_thread_context_available();
    }

    bool DumpFileStreamThreadContext::Wow64ThreadContextAvailable() const
    {
        return exception_.thread_context().wow64_thread_context_available();
    }

    bool DumpFileStreamThreadContext::X64ThreadContextAvailable() const
    {
        return exception_.thread_context().x64_thread_context_available();
    }

    bool DumpFileStreamThreadContext::X86ThreadContextHasExtendedRegisters() const
    {
        return exception_.thread_context().x86_thread_context_has_extended_registers();
    }

    bool DumpFileStreamThreadContext::Wow64ThreadContextHasExtendedRegisters() const
    {
        return exception_.thread_context().wow64_thread_context_has_extended_registers();
    }

    winrt::MiniDumpExplorer::IDumpFileStreamX86ThreadContext DumpFileStreamThreadContext::X86ThreadContext() const
    {
        return GetResult(x86_thread_context_);
    }

    winrt::MiniDumpExplorer::IDumpFileStreamX86ThreadContext DumpFileStreamThreadContext::Wow64ThreadContext() const
    {
        return GetResult(wow64_thread_context_);
    }

    winrt::MiniDumpExplorer::IDumpFileStreamX64ThreadContext DumpFileStreamThreadContext::X64ThreadContext() const
    {
        return GetResult(x64_thread_context_);
    }
}
