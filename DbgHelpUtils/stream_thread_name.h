﻿#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

// ReSharper disable once CppUnusedIncludeDirective
#include <string_view>

namespace dlg_help_utils
{
    class mini_dump;

    class stream_thread_name
    {
    public:
        stream_thread_name() = default;
        explicit stream_thread_name(mini_dump const& dump, MINIDUMP_THREAD_NAME const& thread);

        [[nodiscard]] bool is_valid() const { return is_valid_; }
        MINIDUMP_THREAD_NAME const* operator->() const { return thread_; }
        [[nodiscard]] std::wstring_view const& name() const { return name_; }

    private:
        bool is_valid_{false};
        MINIDUMP_THREAD_NAME const* thread_{nullptr};
        std::wstring_view name_;
    };
}
