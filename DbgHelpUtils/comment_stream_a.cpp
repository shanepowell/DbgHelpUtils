#include "comment_stream_a.h"

#include <stdexcept>

#include "mini_dump.h"
#include "string_conversation.h"

namespace dlg_help_utils
{
    comment_stream_a::comment_stream_a(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(CommentStreamA, index_);
        if (entry == nullptr)
        {
            return;
        }

        found_ = true;
        comment_ = string_conversation::acp_to_wstring(std::string_view{
            static_cast<char const*>(dump.rva32(entry->Location)), entry->Location.DataSize
        });
    }
}
