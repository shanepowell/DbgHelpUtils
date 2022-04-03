#include "heap_match_utils.h"

#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    namespace
    {
        // assume front padding never goes above this size...
        // this is a made up value as I have never see the front end padding to be very large
        uint64_t constexpr maximum_front_crt_heap_memory_padding = 1024;
    }

    block_range_match_result heap_match_utils::does_memory_match_to_range(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const user_address, size_units::base_16::bytes const user_size, uint64_t const block_address, size_units::base_16::bytes const block_size)
    {
        if(user_address == block_address && user_size == block_size)
        {
            return block_range_match_result::block_match;
        }

        if(auto const front_padding_size = read_front_padding_size(walker, block_address, block_size);
            user_address == block_address + front_padding_size && user_size == block_size - size_units::base_16::bytes{front_padding_size})
        {
            return block_range_match_result::block_match;
        }

        if(user_address >= block_address && user_address + user_size.count() <= block_address + block_size.count())
        {
            return block_range_match_result::block_contains;
        }

        if(block_address >= user_address  && block_address + block_size.count() <= user_address + user_size.count())
        {
            return block_range_match_result::user_contains_block;
        }

        if(user_address < block_address + block_size.count() && user_address + user_size.count() > block_address)
        {
            return block_range_match_result::block_partially_contains;
        }

        return block_range_match_result::block_no_match;
    }

    uint64_t heap_match_utils::read_front_padding_size(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const block_address, size_units::base_16::bytes const block_size)
    {
        auto const value = stream_utils::read_field_value<uint32_t>(walker, block_address).value_or(0);
        if(value >= block_size.count() ||
            value > maximum_front_crt_heap_memory_padding)
        {
            return 0;
        }

        return value;
    }
}
