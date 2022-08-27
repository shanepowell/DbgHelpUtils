#pragma once
#include <cstdint>

#include "block_range_match_result.h"
#include "size_units.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::heap
{
    enum class heap_node_type;

    class heap_match_utils
    {
    public:
        static [[nodiscard]] block_range_match_result does_memory_match_to_range(uint64_t user_address
            , size_units::base_16::bytes user_size
            , uint64_t block_address
            , size_units::base_16::bytes block_size);
    };
}
