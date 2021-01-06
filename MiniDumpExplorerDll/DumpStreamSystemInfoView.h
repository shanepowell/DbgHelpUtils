#pragma once

#include "DumpStreamSystemInfoView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamSystemInfoView : DumpStreamSystemInfoViewT<DumpStreamSystemInfoView>
    {
        DumpStreamSystemInfoView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamSystemInfoView : DumpStreamSystemInfoViewT<DumpStreamSystemInfoView, implementation::DumpStreamSystemInfoView>
    {
    };
}
