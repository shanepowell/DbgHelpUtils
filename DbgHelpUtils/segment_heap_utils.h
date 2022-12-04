#pragma once
#include <optional>

#include "size_units.h"

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
        static segment_heap_entry_extra_data read_extra_data(process::process_environment_block const& peb, ust_address_stack_trace const& stack_trace, uint64_t block_address, uint64_t block_size);
        static uint64_t read_ust_address(process::process_environment_block const& peb, ust_address_stack_trace const& stack_trace, uint64_t block_address, uint64_t block_size, uint64_t unused_bytes);

        static std::optional<uint64_t> read_front_padding_size(process::process_environment_block const& peb, uint64_t block_address, uint64_t block_size, uint16_t windows10_min_version, uint16_t windows10_max_version);
        static std::optional<uint64_t> read_front_padding_size_large(process::process_environment_block const& peb, uint64_t block_address, uint64_t block_size, uint16_t windows10_min_version, uint16_t windows10_max_version);

        static bool may_have_front_padding(process::process_environment_block const& peb, uint16_t windows10_min_version, uint16_t windows10_max_version);

    private:
        template<size_t N>
        static std::optional<uint64_t> get_read_front_padding_size(process::process_environment_block const& peb, uint64_t block_address, uint64_t block_size, std::array<uint64_t, N> front_padding_sizes);
    };
}
