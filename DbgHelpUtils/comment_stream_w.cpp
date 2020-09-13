#include "comment_stream_w.h"

#include <stdexcept>

#include "mini_dump.h"

namespace dlg_help_utils
{
    comment_stream_w::comment_stream_w(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(CommentStreamW, index_);
        if (entry == nullptr)
        {
            return;
        }

        if ((entry->Location.DataSize % 2) != 0)
        {
            throw std::runtime_error{"invalid CommentStreamW data"};
        }

        found_ = true;
        comment_ = std::wstring_view{
            static_cast<wchar_t const*>(dump.rva32(entry->Location)), entry->Location.DataSize / 2
        };
    }
}
