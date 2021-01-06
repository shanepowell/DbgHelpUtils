#pragma once

#include "DumpStreamCommentView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamCommentView : DumpStreamCommentViewT<DumpStreamCommentView>
    {
        DumpStreamCommentView(IDumpFileStreamComment stream);

        IDumpFileStreamComment Stream() const;

    private:
        IDumpFileStreamComment stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamCommentView : DumpStreamCommentViewT<DumpStreamCommentView, implementation::DumpStreamCommentView>
    {
    };
}
