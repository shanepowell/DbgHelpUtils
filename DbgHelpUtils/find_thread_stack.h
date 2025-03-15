#pragma once
#include <cstdint>
#include <optional>

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

namespace dlg_help_utils
{
    class mini_dump;

    struct thread_stack
    {
        size_t list_stream_index{};
        size_t stream_index{};
        MINIDUMP_STREAM_TYPE stream_type{};
        void const* stack{nullptr};
        uint32_t stack_size{};
        uint64_t stack_start_address{};
    };

    std::optional<thread_stack> find_thread_stack(mini_dump const& mini_dump, uint32_t thread_id);
};
