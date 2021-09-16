#pragma once
#include <cstdint>

#include "size_units.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::heap
{
    class crt_heap;

    class crt_entry
    {
    public:
        crt_entry(crt_heap const& heap, uint64_t entry_address);

        [[nodiscard]] crt_heap const& heap() const { return heap_; }
        [[nodiscard]] process::process_environment_block const& peb() const;
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;

        [[nodiscard]] uint64_t entry_address() const { return entry_address_; }

        [[nodiscard]] std::wstring filename() const { return file_name_; }
        [[nodiscard]] uint32_t line_number() const;
        [[nodiscard]] bool block_use() const;
        [[nodiscard]] size_units::base_16::bytes data_size() const;
        [[nodiscard]] uint32_t request_number() const;
        [[nodiscard]] uint64_t user_address() const;

        [[nodiscard]] uint64_t symbol_address() const { return entry_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return crt_mem_block_header_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] std::wstring get_filename() const;

    private:
        crt_heap const& heap_;
        uint64_t const entry_address_;
        dbg_help::symbol_type_info const crt_mem_block_header_symbol_type_;
        uint64_t const crt_mem_block_header_length_;
        std::wstring const file_name_;
    };
}
