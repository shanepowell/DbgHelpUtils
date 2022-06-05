#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include <compare>
#include <cstdint>

#include "size_units.h"

namespace dlg_help_utils
{
    struct memory_range
    {
        uint64_t start_range;
        uint64_t end_range;

        auto operator<=>(memory_range const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)
    };

    inline size_units::base_16::bytes size(memory_range const& range)
    {
        return size_units::base_16::bytes{range.end_range - range.start_range};
    }
}
