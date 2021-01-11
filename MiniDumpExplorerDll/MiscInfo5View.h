#pragma once

#include "MiscInfo5View.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiscInfo5View : MiscInfo5ViewT<MiscInfo5View>
    {
        MiscInfo5View();

        [[nodiscard]] IDumpFileStreamMiscInfo Stream() const;
        void Stream(IDumpFileStreamMiscInfo value);

    private:
        IDumpFileStreamMiscInfo stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiscInfo5View : MiscInfo5ViewT<MiscInfo5View, implementation::MiscInfo5View>
    {
    };
}
