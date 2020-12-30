#pragma once

#include "DumpHeader.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpHeader : DumpHeaderT<DumpHeader>
    {
        DumpHeader(IDumpFileHeader header);

        IDumpFileHeader Header() const;
        hstring HeaderType() const;
        bool IsKernelDumpFile() const;
        bool IsUserDumpFile() const;
        bool IsInvalidDumpFile() const;
        bool IsUserOrInvalidDumpFile() const;

    private:
        IDumpFileHeader header_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpHeader : DumpHeaderT<DumpHeader, implementation::DumpHeader>
    {
    };
}
