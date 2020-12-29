#include "pch.h"
#include "DumpFileFactory.h"

namespace MiniDumpExplorerApp
{
    DumpFileFactory::DumpFileFactory(dlg_help_utils::dbg_help::symbol_engine& symbol_engine)
        : symbol_engine_{symbol_engine}
    {
    }

    winrt::MiniDumpExplorer::IDumpFile DumpFileFactory::Make(winrt::hstring path) const
    {
        auto const dump = winrt::make<DumpFile>(std::move(path), symbol_engine_);
        auto file = dump.as<winrt::MiniDumpExplorer::IDumpFile>();
        return file;
    }
}
