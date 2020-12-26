#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>
#include <experimental/generator>

#include "stream_thread_info.h"

namespace dlg_help_utils
{
    class mini_dump;

    class thread_info_list_stream
    {
    public:
        explicit thread_info_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] ULONG64 size() const { return thread_info_list_->NumberOfEntries; }

        [[nodiscard]] std::experimental::generator<stream_thread_info> list() const;

    private:
        mini_dump const& dump_;
        bool found_{false};
        bool is_valid_{false};
        size_t index_;
        MINIDUMP_THREAD_INFO_LIST const* thread_info_list_{nullptr};
        MINIDUMP_THREAD_INFO const* list_{nullptr};
    };
}
