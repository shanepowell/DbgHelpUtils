#pragma once

#include "MiscInfo3View.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiscInfo3View : MiscInfo3ViewT<MiscInfo3View>
    {
        MiscInfo3View();

        [[nodiscard]] IDumpFileStreamMiscInfo Stream() const;
        void Stream(IDumpFileStreamMiscInfo value);

    private:
        IDumpFileStreamMiscInfo stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiscInfo3View : MiscInfo3ViewT<MiscInfo3View, implementation::MiscInfo3View>
    {
    };
}
