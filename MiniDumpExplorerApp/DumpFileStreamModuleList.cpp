#include "pch.h"
#include "DumpFileStreamModuleList.h"
#include "MiniDumpModule.h"

namespace MiniDumpExplorerApp
{
    DumpFileStreamModuleList::DumpFileStreamModuleList(size_t const index, dlg_help_utils::mini_dump const& mini_dump)
        : module_list_{mini_dump, index}
    {
        for (auto const& stream_module : module_list_.list())
        {
            winrt::MiniDumpExplorer::MiniDumpModuleProxy proxy{*winrt::make_self<MiniDumpModule>(stream_module)};
            modules_.Append(proxy);
            /*
            modules_.Append(*winrt::make_self<MiniDumpModule>(stream_module));
        */
        }
    }

    uint64_t DumpFileStreamModuleList::Index() const
    {
        return module_list_.index();
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::MiniDumpExplorer::MiniDumpModuleProxy> DumpFileStreamModuleList::Modules() const
    {
        return modules_;
    }
}
