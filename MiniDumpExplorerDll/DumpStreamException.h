#pragma once

#include "DumpStreamException.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpStreamException : DumpStreamExceptionT<DumpStreamException>
    {
        DumpStreamException(IDumpFileStreamException stream);

        IDumpFileStreamException Stream() const;

    private:
        IDumpFileStreamException stream_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpStreamException : DumpStreamExceptionT<DumpStreamException, implementation::DumpStreamException>
    {
    };
}
