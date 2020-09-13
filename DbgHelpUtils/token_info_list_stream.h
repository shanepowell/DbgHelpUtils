#pragma once
#define WIN32_LEAN_AND_MEAN
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>
#include <experimental/generator>

#include "stream_token_info.h"

namespace dlg_help_utils
{
    class mini_dump;

    class token_info_list_stream
    {
    public:
        explicit token_info_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] ULONG64 size() const { return token_info_list_->TokenListEntries; }

        [[nodiscard]] std::experimental::generator<stream_token_info> list() const;

    private:
        bool found_{false};
        bool is_valid_{false};
        size_t index_;
        void const* end_list_{nullptr};
        MINIDUMP_TOKEN_INFO_LIST const* token_info_list_{nullptr};
        MINIDUMP_TOKEN_INFO_HEADER const* list_{nullptr};
    };
}
