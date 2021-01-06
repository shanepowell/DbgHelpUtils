#pragma once

#include "DumpStreamExceptionView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamExceptionView : DumpStreamExceptionViewT<DumpStreamExceptionView>
    {
        DumpStreamExceptionView(IDumpFileStreamException stream);

        IDumpFileStreamException Stream() const;

    private:
        IDumpFileStreamException stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamExceptionView : DumpStreamExceptionViewT<DumpStreamExceptionView, implementation::DumpStreamExceptionView>
    {
    };
}
