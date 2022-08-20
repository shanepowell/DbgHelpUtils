#pragma once
#include <algorithm>
#include <cstdint>

namespace dlg_help_utils
{
    template<typename T>
    uint64_t get_last_address(T entry)
    {
        return entry.user_address() + std::max(entry.user_requested_size().count(), 1LL) - 1;
    }
}
