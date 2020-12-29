#include "pch.h"
#include "DumpFileStream.h"

#include "../DbgHelpUtils/mini_dump_stream_type.h"

namespace MiniDumpExplorerApp
{

    DumpFileStream::DumpFileStream(size_t index, MINIDUMP_DIRECTORY const& directory_entry)
    : index_{index}
    , directory_entry_{directory_entry}
    {
    }

    uint64_t DumpFileStream::Index() const
    {
        return index_;
    }

    winrt::MiniDumpExplorer::StreamType DumpFileStream::Type() const
    {
        return static_cast<winrt::MiniDumpExplorer::StreamType>(directory_entry_.StreamType);
    }

    winrt::hstring DumpFileStream::TypeString() const
    {
        return winrt::hstring{dlg_help_utils::mini_dump_stream_type::to_string(static_cast<MINIDUMP_STREAM_TYPE>(directory_entry_.StreamType))};
    }
}
