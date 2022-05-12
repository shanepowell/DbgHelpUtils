#pragma once
#include <cstdint>
#include <vector>

#include "size_units.h"
#include "stream_utils.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::heap
{
    class dph_heap;

    class dph_entry
    {
    public:
        static constexpr uint32_t StampAllocNormalPageMode = 0xABCDAAAA;
        static constexpr uint32_t StampFreeNormalPageMode = 0xABCDAAA9;
        static constexpr uint32_t StampAllocFullPageMode = 0xABCDBBBB;
        static constexpr uint32_t StampFreeFullPageMode = 0xABCDBBBA;


        dph_entry(dph_heap const& heap, uint64_t entry_address);

        [[nodiscard]] uint64_t entry_address() const { return entry_address_; }
        [[nodiscard]] dph_heap const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t virtual_block_address() const { return virtual_block_address_; }
        [[nodiscard]] size_units::base_16::bytes virtual_block_size() const { return virtual_block_size_; }
        [[nodiscard]] uint64_t user_address() const { return user_address_; }
        [[nodiscard]] size_units::base_16::bytes user_requested_size() const { return user_requested_size_; }
        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }
        [[nodiscard]] bool is_allocated() const { return allocated_; }
        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }

        [[nodiscard]] uint64_t next_alloc_address() const { return next_alloc_address_; }

        [[nodiscard]] uint64_t symbol_address() const { return entry_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->dph_heap_block_symbol_type; }

        static std::wstring const& symbol_name;
        static std::wstring const& block_info_symbol_name;
        static void setup_globals(dph_heap const& heap);

    private:
        [[nodiscard]] uint64_t get_virtual_block_address() const;
        [[nodiscard]] size_units::base_16::bytes get_virtual_block_size() const;
        [[nodiscard]] uint64_t get_user_address() const;
        [[nodiscard]] size_units::base_16::bytes get_user_requested_size() const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] uint64_t get_next_alloc_address() const;
        [[nodiscard]] bool get_is_allocated() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info dph_heap_block_symbol_type;
            dbg_help::symbol_type_info dph_block_information_symbol_type;
            size_t dph_block_information_symbol_length{};
            stream_utils::symbol_type_and_base_type_field_offset dph_block_information_start_stamp_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_block_virtual_block_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_block_user_requested_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_block_virtual_block_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_block_user_allocation_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_block_stack_trace_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_block_next_alloc_field_data;
        };

        cache_data const* cache_data_;
        dph_heap const* heap_;
        uint64_t entry_address_;
        uint64_t virtual_block_address_{get_virtual_block_address()};
        size_units::base_16::bytes virtual_block_size_{get_virtual_block_size()};
        uint64_t user_address_{get_user_address()};
        size_units::base_16::bytes user_requested_size_{get_user_requested_size()};
        uint64_t ust_address_{get_ust_address()};
        uint64_t next_alloc_address_{get_next_alloc_address()};
        bool allocated_{get_is_allocated()};
        std::vector<uint64_t> allocation_stack_trace_{get_allocation_stack_trace()};
    };

}
