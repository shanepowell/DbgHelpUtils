#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>

#include "stream_thread_context.h"
#include "stream_thread_name.h"

namespace dlg_help_utils
{
    class memory64_list_stream;
    class memory_list_stream;
    class thread_names_list_stream;
    class mini_dump;

    class stream_thread
    {
    public:
        explicit stream_thread(mini_dump const& dump, MINIDUMP_THREAD const& thread,
                               thread_names_list_stream const& names_list, memory_list_stream const& memory_list,
                               memory64_list_stream const& memory64_list);

        MINIDUMP_THREAD const* operator->() const { return &thread_; }
        [[nodiscard]] void const* stack() const { return stack_; }
        [[nodiscard]] stream_thread_context const& thread_context() const { return thread_context_; }
        [[nodiscard]] std::wstring_view const& thread_name() const { return thread_name_.name(); }

    private:
        MINIDUMP_THREAD const& thread_;
        void const* stack_{nullptr};
        stream_thread_context thread_context_{};
        stream_thread_name thread_name_;
    };
}
