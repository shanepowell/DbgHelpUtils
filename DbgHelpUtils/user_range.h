#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include <compare>
#include <cstdint>

namespace dlg_help_utils
{
    struct user_range
    {
        uint64_t start;
        uint64_t size;

        auto operator<=>(user_range const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)
    };
}
