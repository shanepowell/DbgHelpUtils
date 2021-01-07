#pragma once

#include "DumpStreamSystemInfoView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamSystemInfoView : DumpStreamSystemInfoViewT<DumpStreamSystemInfoView>
    {
        DumpStreamSystemInfoView(IDumpFileStreamSystemInfo system);

        IDumpFileStreamSystemInfo Stream() const;

    private:
        IDumpFileStreamSystemInfo stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamSystemInfoView : DumpStreamSystemInfoViewT<DumpStreamSystemInfoView, implementation::DumpStreamSystemInfoView>
    {
    };
}
