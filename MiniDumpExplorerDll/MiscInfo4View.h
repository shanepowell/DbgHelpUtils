#pragma once

#include "MiscInfo4View.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiscInfo4View : MiscInfo4ViewT<MiscInfo4View>
    {
        MiscInfo4View();

        [[nodiscard]] IDumpFileStreamMiscInfo Stream() const;
        void Stream(IDumpFileStreamMiscInfo value);

    private:
        IDumpFileStreamMiscInfo stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiscInfo4View : MiscInfo4ViewT<MiscInfo4View, implementation::MiscInfo4View>
    {
    };
}
