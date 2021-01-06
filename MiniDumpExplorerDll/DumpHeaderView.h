#pragma once

#include "DumpHeaderView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpHeaderView : DumpHeaderViewT<DumpHeaderView>
    {
        DumpHeaderView(IDumpFileHeader header);

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
    struct DumpHeaderView : DumpHeaderViewT<DumpHeaderView, implementation::DumpHeaderView>
    {
    };
}
