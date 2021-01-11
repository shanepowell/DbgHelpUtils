#pragma once

#include "DumpStreamMiscInfoView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamMiscInfoView : DumpStreamMiscInfoViewT<DumpStreamMiscInfoView>
    {
        DumpStreamMiscInfoView(IDumpFileStreamMiscInfo system);

        [[nodiscard]] IDumpFileStreamMiscInfo Stream() const;

    private:
        IDumpFileStreamMiscInfo stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamMiscInfoView : DumpStreamMiscInfoViewT<DumpStreamMiscInfoView, implementation::DumpStreamMiscInfoView>
    {
    };
}
