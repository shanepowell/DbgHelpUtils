#pragma once
#include "../DbgHelpUtils/mini_dump.h"
#include "../DbgHelpUtils/module_list_stream.h"

namespace MiniDumpExplorerApp
{
    struct DumpFileStreamModuleList : winrt::implements<DumpFileStreamModuleList, winrt::MiniDumpExplorer::IDumpFileStreamModuleList>
    {
        DumpFileStreamModuleList(size_t index, dlg_help_utils::mini_dump const& mini_dump);

        [[nodiscard]] uint64_t Index() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::MiniDumpExplorer::MiniDumpModuleProxy> Modules() const;

    private:
        dlg_help_utils::module_list_stream module_list_;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::MiniDumpExplorer::MiniDumpModuleProxy> modules_{winrt::single_threaded_observable_vector<winrt::MiniDumpExplorer::MiniDumpModuleProxy>()};
    };
}
