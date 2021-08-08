#include "pch.h"
#include "DumpFileStreamComment.h"

#include "../DbgHelpUtils/comment_stream_a.h"
#include "../DbgHelpUtils/comment_stream_w.h"

namespace MiniDumpExplorerApp
{

    DumpFileStreamComment::DumpFileStreamComment(size_t const index, dlg_help_utils::mini_dump const& mini_dump, CommentType const type)
        : index_{index}
        , type_{type}
    {
        switch(type)
        {
        case CommentType::Ascii:
            InitAsciiComment(index, mini_dump);
            break;
        case CommentType::Unicode:
            InitUnicodeComment(index, mini_dump);
            break;
        }
    }

    uint64_t DumpFileStreamComment::Index() const
    {
        return index_;
    }

    bool DumpFileStreamComment::IsAscii() const
    {
        return type_ == CommentType::Ascii;
    }

    bool DumpFileStreamComment::IsUnicode() const
    {
        return type_ == CommentType::Unicode;
    }

    winrt::hstring DumpFileStreamComment::Comment() const
    {
        return comment_;
    }

    void DumpFileStreamComment::InitAsciiComment(size_t const index, dlg_help_utils::mini_dump const& mini_dump)
    {
        dlg_help_utils::comment_stream_a const comment{mini_dump, index};

        if (!comment.found())
        {
            return;
        }

        comment_ = winrt::hstring{comment.comment()};
    }

    void DumpFileStreamComment::InitUnicodeComment(size_t const index, dlg_help_utils::mini_dump const& mini_dump)
    {
        dlg_help_utils::comment_stream_w const comment{mini_dump, index};

        if (!comment.found())
        {
            return;
        }

        comment_ = winrt::hstring{comment.comment()};
    }
}
