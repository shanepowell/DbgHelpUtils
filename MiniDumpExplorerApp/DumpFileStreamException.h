#pragma once
#include "../DbgHelpUtils/exception_stream.h"
#include "../DbgHelpUtils/symbol_engine.h"

namespace MiniDumpExplorerApp
{
    struct DumpFileStreamException : winrt::implements<DumpFileStreamException, winrt::MiniDumpExplorer::IDumpFileStreamException>
    {
        DumpFileStreamException(size_t index, dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);

        [[nodiscard]] uint64_t Index() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] uint32_t ThreadId() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpException ExceptionRecord() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IDumpFileStreamThreadContext ThreadContext() const;

    private:
        dlg_help_utils::dbg_help::symbol_engine& symbol_engine_;
        dlg_help_utils::exception_stream exception_;
    };
}
