#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

namespace dlg_help_utils
{
    class mini_dump;

    class system_memory_info_stream
    {
    public:
        explicit system_memory_info_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] size_t system_memory_info_version() const { return system_memory_info_version_; }

        [[nodiscard]] MINIDUMP_SYSTEM_MEMORY_INFO_1 const& system_memory_misc_info() const
        {
            return *system_memory_info_;
        }

    private:
        bool found_{false};
        bool is_valid_{false};
        size_t index_{};
        size_t system_memory_info_version_{};
        MINIDUMP_SYSTEM_MEMORY_INFO_1 const* system_memory_info_{nullptr};
    };
}
