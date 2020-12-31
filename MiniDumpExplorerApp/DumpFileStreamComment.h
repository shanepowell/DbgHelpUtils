#pragma once
#include "../DbgHelpUtils/mini_dump.h"

namespace MiniDumpExplorerApp
{
    enum class CommentType
    {
        Ascii,
        Unicode
    };

    struct DumpFileStreamComment : winrt::implements<DumpFileStreamComment, winrt::MiniDumpExplorer::IDumpFileStreamComment>
    {
        DumpFileStreamComment(size_t index, dlg_help_utils::mini_dump const& mini_dump, CommentType type);

        [[nodiscard]] uint64_t Index() const;
        [[nodiscard]] bool IsAscii() const;
        [[nodiscard]] bool IsUnicode() const;
        [[nodiscard]] winrt::hstring Comment() const;

    private:
        void InitAsciiComment(size_t index, dlg_help_utils::mini_dump const& mini_dump);
        void InitUnicodeComment(size_t index, dlg_help_utils::mini_dump const& mini_dump);

    private:
        uint64_t index_;
        CommentType type_;
        winrt::hstring comment_;
    };
}
