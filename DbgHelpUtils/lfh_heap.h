#pragma once
#include <cstdint>
#include <experimental/generator>

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
    class lfh_segment;
    class nt_heap;

    class lfh_heap
    {
    public:
        lfh_heap(nt_heap const& heap, uint64_t lfh_heap_address);

        [[nodiscard]] nt_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t address() const { return lfh_heap_address_; }

        [[nodiscard]] std::optional<uint64_t> const& lfh_key() const { return cache_data_.lfh_key; }
        [[nodiscard]] std::experimental::generator<lfh_segment> lfh_segments() const;

        [[nodiscard]] uint64_t symbol_address() const { return address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_.lfh_heap_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(nt_heap const& heap);

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info lfh_heap_symbol_type;
            std::optional<uint64_t> lfh_key;
            dbg_help::symbol_type_and_field_offset lfh_heap_sub_segment_zones_field_data;
        };

        cache_data const& cache_data_;
        nt_heap const& heap_;
        uint64_t const lfh_heap_address_;
    };
}
