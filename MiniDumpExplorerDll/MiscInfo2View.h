#pragma once

#include "MiscInfo2View.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiscInfo2View : MiscInfo2ViewT<MiscInfo2View>
    {
        MiscInfo2View();

        [[nodiscard]] IDumpFileStreamMiscInfo Stream() const;
        void Stream(IDumpFileStreamMiscInfo value);

    private:
        IDumpFileStreamMiscInfo stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiscInfo2View : MiscInfo2ViewT<MiscInfo2View, implementation::MiscInfo2View>
    {
    };
}
