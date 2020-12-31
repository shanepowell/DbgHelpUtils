#include "Pch.h"
#include "DumpFileStream.h"

#include "../DbgHelpUtils/mini_dump_stream_type.h"

namespace MiniDumpExplorerApp
{

    DumpFileStream::DumpFileStream(size_t index, MINIDUMP_DIRECTORY const& directory_entry, dlg_help_utils::mini_dump const& mini_dump)
    : index_{index}
    , directory_entry_{directory_entry}
    , mini_dump_{mini_dump}
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

    winrt::MiniDumpExplorer::IDumpFileStreamComment DumpFileStream::AsComment() const
    {
        switch(static_cast<MINIDUMP_STREAM_TYPE>(directory_entry_.StreamType))
        {
        case CommentStreamA:
            return *winrt::make_self<DumpFileStreamComment>(index_, mini_dump_, CommentType::Ascii);

        case CommentStreamW:
            return *winrt::make_self<DumpFileStreamComment>(index_, mini_dump_, CommentType::Unicode);

        default:
            throw winrt::hresult_illegal_method_call();
        }
    }
}
