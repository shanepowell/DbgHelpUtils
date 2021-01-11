#pragma once

#include "MiscInfoView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiscInfoView : MiscInfoViewT<MiscInfoView>
    {
        MiscInfoView();

        [[nodiscard]] IDumpFileStreamMiscInfo Stream() const;
        void Stream(IDumpFileStreamMiscInfo value);

    private:
        IDumpFileStreamMiscInfo stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiscInfoView : MiscInfoViewT<MiscInfoView, implementation::MiscInfoView>
    {
    };
}
