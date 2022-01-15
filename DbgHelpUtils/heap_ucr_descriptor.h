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
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::heap
{
    class nt_heap;

    class heap_ucr_descriptor
    {
    public:
        heap_ucr_descriptor(nt_heap const& heap, uint64_t heap_ucr_descriptor_address);

        [[nodiscard]] nt_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t address() const;
        [[nodiscard]] size_units::base_16::bytes size() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_ucr_descriptor_address_; }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_.heap_ucr_descriptor_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(nt_heap const& heap);

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_ucr_descriptor_symbol_type;
            stream_utils::symbol_type_and_base_type_field_offset heap_ucr_descriptor_address_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_ucr_descriptor_size_field_data;
        };

        cache_data const& cache_data_;
        nt_heap const& heap_;
        uint64_t const heap_ucr_descriptor_address_;
    };
}
