#include "heap_match_utils.h"

#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    block_range_match_result heap_match_utils::does_memory_match_to_range(uint64_t const user_address
        , size_units::base_16::bytes const user_size
        , uint64_t const block_address
        , size_units::base_16::bytes const block_size)
    {
        if(user_address == block_address && user_size == block_size)
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
}
