#pragma once
#define WIN32_LEAN_AND_MEAN
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>

#include <string_view>

#include "stream_thread_context.h"
#include "stream_thread_name.h"

namespace dlg_help_utils
{
    class thread_names_list_stream;
    class mini_dump;

    class stream_thread_ex
    {
    public:
        explicit stream_thread_ex(mini_dump const& dump, MINIDUMP_THREAD_EX const& thread,
                                  thread_names_list_stream const& names_list);

        MINIDUMP_THREAD_EX const* operator->() const { return &thread_; }
        [[nodiscard]] void const* stack() const { return stack_; }
        [[nodiscard]] stream_thread_context const& thread_context() const { return thread_context_; }
        [[nodiscard]] void const* backing_store() const { return backing_store_; }
        [[nodiscard]] std::wstring_view const& thread_name() const { return thread_name_.name(); }

    private:
        MINIDUMP_THREAD_EX const& thread_;
        void const* stack_{nullptr};
        stream_thread_context thread_context_{};
        void const* backing_store_{nullptr};
        stream_thread_name thread_name_;
    };
}
