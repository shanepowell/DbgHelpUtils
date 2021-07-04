#pragma once
#include "../DbgHelpUtils/exception_stream.h"
#include "../DbgHelpUtils/symbol_engine.h"

class DbgHelpDispatcher;
struct MiniDumpData;

namespace MiniDumpExplorerApp
{
    struct DumpFileStreamException : winrt::implements<DumpFileStreamException, winrt::MiniDumpExplorer::IDumpFileStreamException>
    {
        DumpFileStreamException(size_t index, dlg_help_utils::mini_dump const& mini_dump, DbgHelpDispatcher& dispatcher, MiniDumpData& mini_data_data, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);

        [[nodiscard]] uint64_t Index() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] uint32_t ThreadId() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpException ExceptionRecord() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IDumpFileStreamThreadContext ThreadContext() const;
        [[nodiscard]] bool HasStackTrace() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IStackTrace StackTrace() const;

    private:
        dlg_help_utils::exception_stream exception_;
        winrt::MiniDumpExplorer::IMiniDumpException mini_dump_exception_;
        winrt::MiniDumpExplorer::IDumpFileStreamThreadContext thread_context_;
        winrt::MiniDumpExplorer::IStackTrace stack_trace_;
    };
}
