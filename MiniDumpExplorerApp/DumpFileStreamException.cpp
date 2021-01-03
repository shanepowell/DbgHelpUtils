#include "pch.h"
#include "DumpFileStreamException.h"


#include "DumpFileStreamThreadContext.h"
#include "MiniDumpException.h"

namespace MiniDumpExplorerApp
{
    DumpFileStreamException::DumpFileStreamException(size_t const index, dlg_help_utils::mini_dump const& mini_dump, dlg_help_utils::dbg_help::symbol_engine& symbol_engine)
        : symbol_engine_{symbol_engine}
        , exception_{mini_dump, index}
    {
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
        return *winrt::make_self<MiniDumpException>(exception_);
    }

    winrt::MiniDumpExplorer::IDumpFileStreamThreadContext DumpFileStreamException::ThreadContext() const
    {
        return *winrt::make_self<DumpFileStreamThreadContext>(exception_);
    }
}
