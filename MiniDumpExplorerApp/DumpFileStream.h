#pragma once
#include "../DbgHelpUtils/mini_dump.h"

namespace MiniDumpExplorerApp
{
    struct DumpFileStream : winrt::implements<DumpFileStream, winrt::MiniDumpExplorer::IDumpFileStream>
    {
        DumpFileStream(size_t index, MINIDUMP_DIRECTORY const& directory_entry);

        uint64_t Index() const;
        winrt::MiniDumpExplorer::StreamType Type() const;
        winrt::hstring TypeString() const;

    private:
        size_t index_;
        MINIDUMP_DIRECTORY const& directory_entry_;
    };
}
