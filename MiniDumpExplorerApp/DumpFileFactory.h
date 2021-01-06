#pragma once
#include "DumpFile.h"

class DbgHelpDispatcher;

namespace MiniDumpExplorerApp
{
    struct DumpFileFactory : winrt::implements<DumpFileFactory, winrt::MiniDumpExplorer::IDumpFileFactory>
    {
        DumpFileFactory(DbgHelpDispatcher& dispatcher, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);
        [[nodiscard]] winrt::MiniDumpExplorer::IDumpFile Make(winrt::hstring path) const;

    private:
        DbgHelpDispatcher& dispatcher_; 
        dlg_help_utils::dbg_help::symbol_engine& symbol_engine_;
    };
}
