#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>

namespace dlg_help_utils
{
    class mini_dump;

    class stream_thread_context
    {
    public:
        stream_thread_context() = default;
        explicit stream_thread_context(mini_dump const& dump, MINIDUMP_LOCATION_DESCRIPTOR const& location);

        [[nodiscard]] void const* context() const { return context_; }
        [[nodiscard]] size_t size() const { return size_; }
        [[nodiscard]] bool x86_thread_context_available() const { return x86_thread_context_available_; }
        [[nodiscard]] WOW64_CONTEXT const& x86_thread_context() const { return *x86_thread_context_; }
        [[nodiscard]] bool wow64_thread_context_available() const { return wow64_thread_context_available_; }
        [[nodiscard]] WOW64_CONTEXT const& wow64_thread_context() const { return *wow64_thread_context_; }
        [[nodiscard]] bool x64_thread_context_available() const { return x64_thread_context_available_; }
        [[nodiscard]] CONTEXT const& x64_thread_context() const { return *x64_thread_context_; }

        [[nodiscard]] bool x86_thread_context_has_extended_registers() const
        {
            return x86_thread_context_has_extended_registers_;
        }

        [[nodiscard]] bool wow64_thread_context_has_extended_registers() const
        {
            return wow64_thread_context_has_extended_registers_;
        }

    private:
        void const* context_{nullptr};
        size_t size_{};
        bool x86_thread_context_available_{false};
        WOW64_CONTEXT const* x86_thread_context_{nullptr};
        bool wow64_thread_context_available_{false};
        WOW64_CONTEXT const* wow64_thread_context_{nullptr};
        bool x64_thread_context_available_{false};
        CONTEXT const* x64_thread_context_{nullptr};
        bool x86_thread_context_has_extended_registers_{false};
        bool wow64_thread_context_has_extended_registers_{false};
    };
}
