#pragma once
#include "../DbgHelpUtils/mini_dump.h"

namespace MiniDumpExplorerApp
{
    struct DumpFileStream : winrt::implements<DumpFileStream, winrt::MiniDumpExplorer::IDumpFileStream>
    {
        DumpFileStream(size_t index, MINIDUMP_DIRECTORY const& directory_entry);

        [[nodiscard]] uint64_t Index() const;
        [[nodiscard]] winrt::MiniDumpExplorer::StreamType Type() const;
        [[nodiscard]] winrt::hstring TypeString() const;

    private:
        size_t index_;
        MINIDUMP_DIRECTORY const& directory_entry_;
    };
}
