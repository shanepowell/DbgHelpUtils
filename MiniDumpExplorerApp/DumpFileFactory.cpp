#include "pch.h"
#include "DumpFileFactory.h"

namespace MiniDumpExplorerApp
{
    DumpFileFactory::DumpFileFactory(DbgHelpDispatcher& dispatcher, dlg_help_utils::dbg_help::symbol_engine& symbol_engine)
    : dispatcher_{dispatcher}
    , symbol_engine_{symbol_engine}
    {
    }

    winrt::MiniDumpExplorer::IDumpFile DumpFileFactory::Make(winrt::hstring path) const
    {
        return *winrt::make_self<DumpFile>(std::move(path), dispatcher_, symbol_engine_);
    }
}
