#pragma once
#include <cstdint>

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
    class nt_heap;
    class heap_entry;
    class lfh_heap;

    class heap_subsegment
    {
    public:
        heap_subsegment(lfh_heap const& heap, uint64_t heap_subsegment_address, uint64_t lfh_block_zone_size);

        [[nodiscard]] lfh_heap const& lfh_heap() const { return *lfh_heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t address() const { return heap_subsegment_address_; }
        [[nodiscard]] size_units::base_16::bytes block_size() const;
        [[nodiscard]] uint16_t block_size_raw() const;
        [[nodiscard]] uint16_t block_count() const;

        [[nodiscard]] uint64_t entry_start_address() const { return entry_start_address_; }
        [[nodiscard]] uint16_t block_stride() const { return block_stride_; }

        [[nodiscard]] std::generator<heap_entry> entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_subsegment_symbol_type; }

        static std::wstring const& symbol_name;
        static std::wstring const& user_data_header_symbol_name;
        static void setup_globals(nt_heap const& heap);

    private:
        [[nodiscard]] std::tuple<uint64_t, uint16_t> get_entry_data() const;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_subsegment_symbol_type;
            dbg_help::symbol_type_info heap_user_data_header_symbol_type;

            stream_utils::symbol_type_and_base_type_field_offset heap_subsegment_block_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_subsegment_block_count_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_subsegment_user_blocks_field_data;

            std::optional<stream_utils::symbol_type_and_base_type_field_offset> heap_user_data_first_allocation_offset_field_data;
            std::optional<stream_utils::symbol_type_and_base_type_field_offset> heap_user_data_encoded_offsets_field_data;
        };

        cache_data const* cache_data_;
        heap::lfh_heap const* lfh_heap_;
        uint64_t heap_subsegment_address_;
        uint64_t lfh_block_zone_size_;
        uint64_t entry_start_address_;
        uint16_t block_stride_;
    };
}
