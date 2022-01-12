#pragma once
#include <utility>

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::heap
{
    class ust_address_stack_trace;

    struct segment_heap_entry_extra_data
    {
        uint64_t unused_bytes;
        uint64_t ust_address;
    };

    class segment_heap_utils
    {
    public:
        static uint64_t read_front_padding_size(process::process_environment_block const& peb, uint64_t block_address);
        static segment_heap_entry_extra_data read_extra_data(process::process_environment_block const& peb, ust_address_stack_trace const& stack_trace, uint64_t block_address, uint64_t block_size);
        static uint64_t read_ust_address(process::process_environment_block const& peb, ust_address_stack_trace const& stack_trace, uint64_t block_address, uint64_t block_size, uint64_t unused_bytes);
    };
}
