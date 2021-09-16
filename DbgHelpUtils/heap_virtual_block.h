#pragma once
#include <cstdint>

#include "size_units.h"
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
    class heap_entry;

    class heap_virtual_block
    {
    public:
        heap_virtual_block(nt_heap const& heap, uint64_t heap_virtual_block_address);

        [[nodiscard]] nt_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t descriptor_address() const { return heap_virtual_block_address_; }
        [[nodiscard]] uint64_t address() const { return data_address_; }
        [[nodiscard]] size_units::base_16::bytes reserved() const;
        [[nodiscard]] size_units::base_16::bytes committed() const;

        [[nodiscard]] std::experimental::generator<heap_entry> entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return descriptor_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_virtual_block_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        nt_heap const& heap_;
        uint64_t const heap_virtual_block_address_;
        dbg_help::symbol_type_info const heap_virtual_block_symbol_type_;
        uint64_t const data_address_;
        uint64_t const busy_block_offset_;
    };
}
