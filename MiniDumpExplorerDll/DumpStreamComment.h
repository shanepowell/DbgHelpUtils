#pragma once

#include "DumpStreamComment.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamComment : DumpStreamCommentT<DumpStreamComment>
    {
        DumpStreamComment(IDumpFileStreamComment stream);

        IDumpFileStreamComment Stream() const;

    private:
        IDumpFileStreamComment stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamComment : DumpStreamCommentT<DumpStreamComment, implementation::DumpStreamComment>
    {
    };
}
