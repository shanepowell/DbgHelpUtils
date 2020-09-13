#pragma once
#include <string>

namespace dlg_help_utils
{
    class mini_dump;

    class comment_stream_a
    {
    public:
        explicit comment_stream_a(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] std::wstring const& comment() const { return comment_; }

    private:
        bool found_{false};
        size_t index_;
        std::wstring comment_;
    };
}
