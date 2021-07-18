#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include "stream_thread_context.h"

namespace dlg_help_utils
{
    class mini_dump;

    class exception_stream
    {
    public:
        explicit exception_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_EXCEPTION_STREAM const& exception() const { return *exception_; }
        [[nodiscard]] stream_thread_context const& thread_context() const { return thread_context_; }

    private:
        bool found_{false};
        size_t index_{};
        MINIDUMP_EXCEPTION_STREAM const* exception_{nullptr};
        stream_thread_context thread_context_{};
    };
}
