#pragma once
#include <cstdint>

#include "lfh_subsegment_location_utils.h"
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
    class segment_heap;
    class heap_lfh_context;
    class heap_lfh_entry;

    class heap_lfh_subsegment
    {
    public:
        heap_lfh_subsegment(heap_lfh_context const& heap, uint64_t heap_lfh_subsegment_address);

        [[nodiscard]] heap_lfh_context const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_lfh_subsegment_address() const { return heap_lfh_subsegment_address_; }

        [[nodiscard]] uint16_t free_count() const;
        [[nodiscard]] uint16_t block_count() const;
        [[nodiscard]] lfh_subsegment_location_utils::location location() const;
        [[nodiscard]] uint8_t witheld_block_count() const;
        [[nodiscard]] size_units::base_16::bytes block_size() const;
        [[nodiscard]] uint8_t commit_unit_shift() const;
        [[nodiscard]] uint8_t commit_unit_count() const;

        [[nodiscard]] generator<heap_lfh_entry> entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_lfh_subsegment_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_lfh_subsegment_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(segment_heap const& heap);

    private:
        template<typename T>
        [[nodiscard]] T get_field_value_from_block_offsets(stream_utils::symbol_type_and_base_type_field_offset const& field_data, std::wstring const& field_name) const;

    private:

        struct cache_data
        {
            dbg_help::symbol_type_info heap_lfh_subsegment_symbol_type;
            dbg_help::symbol_type_info heap_lfh_subsegment_encoded_offsets_symbol_type;
            uint64_t heap_lfh_subsegment_bitmap_offset{};

            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_subsegment_free_count_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_subsegment_block_count_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_subsegment_location_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_subsegment_witheld_block_count_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_subsegment_commit_unit_shift_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_subsegment_commit_unit_count_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_subsegment_block_offsets_field_data;

            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_subsegment_encoded_offsets_block_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_lfh_subsegment_encoded_offsets_first_block_offset_field_data;
        };

        cache_data const* cache_data_;
        heap_lfh_context const* heap_;
        uint64_t heap_lfh_subsegment_address_;
        std::unique_ptr<uint8_t[]> block_offsets_;
    };
}
