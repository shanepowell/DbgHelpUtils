#include "pch.h"
#include "DumpFileStreamException.h"

#include "DumpFileStreamThreadContext.h"
#include "MiniDumpException.h"
#include "StackTrace.h"
#include "../DbgHelpUtils/find_thread_stack.h"

namespace MiniDumpExplorerApp
{
    DumpFileStreamException::DumpFileStreamException(size_t const index, dlg_help_utils::mini_dump const& mini_dump, DbgHelpDispatcher& dispatcher, MiniDumpData& mini_data_data, dlg_help_utils::dbg_help::symbol_engine& symbol_engine)
        : symbol_engine_{symbol_engine}
        , exception_{mini_dump, index}
        , mini_dump_exception_{*winrt::make_self<MiniDumpException>(exception_)}
        , thread_context_{*winrt::make_self<DumpFileStreamThreadContext>(exception_)}
    {
        auto const stack_info = find_thread_stack(mini_dump, exception_.exception().ThreadId);
        if (stack_info)
        {
            stack_trace_ = *winrt::make_self<MiniDumpExplorerApp::StackTrace>(dispatcher, mini_data_data, symbol_engine, stack_info->stack_start_address, stack_info->stack, stack_info->stack_size, exception_.thread_context());
        }
    }

    uint64_t DumpFileStreamException::Index() const
    {
        return exception_.index();
    }

    bool DumpFileStreamException::IsValid() const
    {
        return exception_.found();
    }

    uint32_t DumpFileStreamException::ThreadId() const
    {
        return exception_.exception().ThreadId;
    }

    winrt::MiniDumpExplorer::IMiniDumpException DumpFileStreamException::ExceptionRecord() const
    {
        return mini_dump_exception_;
    }

    winrt::MiniDumpExplorer::IDumpFileStreamThreadContext DumpFileStreamException::ThreadContext() const
    {
        return thread_context_;
    }

    bool DumpFileStreamException::HasStackTrace() const
    {
        return static_cast<bool>(stack_trace_);
    }

    winrt::MiniDumpExplorer::IStackTrace DumpFileStreamException::StackTrace() const
    {
        return stack_trace_;
    }
}
