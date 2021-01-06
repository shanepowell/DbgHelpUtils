#pragma once

#include "DumpStreamMiscInfoView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamMiscInfoView : DumpStreamMiscInfoViewT<DumpStreamMiscInfoView>
    {
        DumpStreamMiscInfoView();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamMiscInfoView : DumpStreamMiscInfoViewT<DumpStreamMiscInfoView, implementation::DumpStreamMiscInfoView>
    {
    };
}
