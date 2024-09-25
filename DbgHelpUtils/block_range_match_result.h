#pragma once

namespace dlg_help_utils::heap
{
    enum class block_range_match_result : uint8_t
    {
        block_match,
        block_contains,
        user_contains_block,
        block_partially_contains,
        block_no_match
    };
}
