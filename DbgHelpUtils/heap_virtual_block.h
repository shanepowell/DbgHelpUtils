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

    class heap_virtual_block
    {
    public:
        heap_virtual_block(nt_heap const& heap, uint64_t heap_virtual_block_address);

        [[nodiscard]] nt_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t descriptor_address() const { return heap_virtual_block_address_; }
        [[nodiscard]] uint64_t address() const { return data_address_; }
        [[nodiscard]] size_units::base_16::bytes reserved() const;
        [[nodiscard]] size_units::base_16::bytes committed() const;

        [[nodiscard]] std::experimental::generator<heap_entry> entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return descriptor_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_.heap_virtual_block_symbol_type; }

        static std::wstring const& symbol_name;
        static void setup_globals(nt_heap const& heap);

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_virtual_block_symbol_type;
            uint64_t heap_virtual_block_length{};
            uint64_t busy_block_offset{};
            stream_utils::symbol_type_and_base_type_field_offset heap_virtual_alloc_entry_reserve_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_virtual_alloc_entry_commit_size_field_data;
        };

        cache_data const& cache_data_;
        nt_heap const& heap_;
        uint64_t const heap_virtual_block_address_;
        uint64_t const data_address_;
    };
}
