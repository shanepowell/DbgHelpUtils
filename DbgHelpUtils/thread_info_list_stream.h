#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <experimental/generator>
#include <limits>

#include "stream_thread_info.h"

namespace dlg_help_utils
{
    class mini_dump;

    class thread_info_list_stream
    {
    public:
        thread_info_list_stream() = default;
        explicit thread_info_list_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] ULONG64 size() const { return thread_info_list_->NumberOfEntries; }

        [[nodiscard]] std::experimental::generator<stream_thread_info> list() const;

    private:
        mini_dump const* dump_{nullptr};
        bool found_{false};
        bool is_valid_{false};
        size_t index_{};
        MINIDUMP_THREAD_INFO_LIST const* thread_info_list_{nullptr};
        MINIDUMP_THREAD_INFO const* list_{nullptr};
    };
}
