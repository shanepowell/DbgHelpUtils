#pragma once
#include <string>
#include <limits>

namespace dlg_help_utils
{
    class mini_dump;

    class comment_stream_w
    {
    public:
        explicit comment_stream_w(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] std::wstring const& comment() const { return comment_; }

    private:
        bool found_{false};
        size_t index_;
        std::wstring comment_;
    };
}
