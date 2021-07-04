#pragma once

#include "DumpStreamModuleListView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamModuleListView : DumpStreamModuleListViewT<DumpStreamModuleListView>
    {
        DumpStreamModuleListView(IDumpFileStreamModuleList stream);

        IDumpFileStreamModuleList Stream() const;

    private:
        IDumpFileStreamModuleList stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamModuleListView : DumpStreamModuleListViewT<DumpStreamModuleListView, implementation::DumpStreamModuleListView>
    {
    };
}
