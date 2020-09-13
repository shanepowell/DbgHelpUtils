#pragma once
#include <cstdint>
#include <optional>

namespace dlg_help_utils
{
    class mini_dump;

    struct thread_stack
    {
        void const* stack{nullptr};
        uint32_t stack_size{};
        uint64_t stack_start_address{};
    };

    std::optional<thread_stack> find_thread_stack(mini_dump const& mini_dump, uint32_t thread_id);
};
