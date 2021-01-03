#pragma once
namespace dlg_help_utils
{
    class exception_stream;
}

namespace MiniDumpExplorerApp
{
    struct DumpFileStreamThreadContext : winrt::implements<DumpFileStreamThreadContext, winrt::MiniDumpExplorer::IDumpFileStreamThreadContext>
    {
        DumpFileStreamThreadContext(dlg_help_utils::exception_stream const& exception);

        [[nodiscard]] bool X86ThreadContextAvailable() const;
        [[nodiscard]] bool Wow64ThreadContextAvailable() const;
        [[nodiscard]] bool X64ThreadContextAvailable() const;
        [[nodiscard]] bool X86ThreadContextHasExtendedRegisters() const;
        [[nodiscard]] bool Wow64ThreadContextHasExtendedRegisters() const;

        [[nodiscard]] winrt::MiniDumpExplorer::IDumpFileStreamX86ThreadContext X86ThreadContext() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IDumpFileStreamX86ThreadContext Wow64ThreadContext() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IDumpFileStreamX64ThreadContext X64ThreadContext() const;

    private:
        dlg_help_utils::exception_stream const& exception_;
    };
}
