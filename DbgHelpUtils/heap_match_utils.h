#pragma once
#include <cstdint>

#include "size_units.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::heap
{
    enum class block_range_match_result
    {
        block_match,
        block_contains,
        user_contains_block,
        block_partially_contains,
        block_no_match
    };

    class heap_match_utils
    {
    public:
        static [[nodiscard]] block_range_match_result does_memory_match_to_range(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t user_address, size_units::base_16::bytes user_size, uint64_t block_address, size_units::base_16::bytes block_size);
        static [[nodiscard]] uint64_t read_front_padding_size(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t block_address, size_units::base_16::bytes block_size);
        
    };
}
