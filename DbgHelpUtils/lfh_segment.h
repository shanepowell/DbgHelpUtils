#pragma once
#include <cstdint>
#include <vector>

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
    class lfh_heap;
    class heap_subsegment;
    class nt_heap;

    class lfh_segment
    {
    public:
        lfh_segment(lfh_heap const& heap, uint64_t lfh_segment_address);

        [[nodiscard]] lfh_heap const& lfh_heap() const { return lfh_heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t address() const { return lfh_segment_address_; }
        [[nodiscard]] size_t subsegments_count() const;
        [[nodiscard]] std::vector<heap_subsegment> const& subsegments() const { return subsegments_; }

        [[nodiscard]] uint64_t symbol_address() const { return address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_.lfh_block_zone_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(nt_heap const& heap);

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info lfh_block_zone_symbol_type;
            size_t heap_subsegment_size{};
            size_t lfh_block_zone_size{};
            std::optional<stream_utils::symbol_type_and_base_type_field_offset> lfh_block_zone_next_index_field_data;
            std::optional<stream_utils::symbol_type_and_base_type_field_offset> lfh_block_zone_free_pointer_field_data;
        };

        [[nodiscard]] std::pair<uint64_t, uint64_t> get_subsegment_range() const;
        [[nodiscard]] std::vector<heap_subsegment> build_subsegments() const;

        [[nodiscard]] static size_t get_lfh_block_zone_size(cache_data const& cache, nt_heap const& heap);

    private:
        cache_data const& cache_data_;
        heap::lfh_heap const& lfh_heap_;
        uint64_t const lfh_segment_address_;
        std::vector<heap_subsegment> const subsegments_{build_subsegments()};
    };
}
