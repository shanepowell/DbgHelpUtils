#pragma once
#include <cstdint>

#include "size_units.h"
#include "stream_utils.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
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
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;

        [[nodiscard]] uint64_t entry_address() const { return entry_address_; }
        [[nodiscard]] uint64_t end_entry_address() const;

        [[nodiscard]] std::wstring filename() const { return file_name_; }
        [[nodiscard]] uint32_t line_number() const;
        [[nodiscard]] bool block_use() const;
        [[nodiscard]] size_units::base_16::bytes data_size() const;
        [[nodiscard]] uint32_t request_number() const;
        [[nodiscard]] uint64_t user_address() const;

        [[nodiscard]] uint64_t symbol_address() const { return entry_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_.crt_mem_block_header_symbol_type; }

        static std::wstring const& symbol_name;

        [[nodiscard]] bool operator==(crt_entry const& rhs) const
        {
            return request_number() == rhs.request_number() && entry_address() == rhs.entry_address() && data_size() == rhs.data_size();
        }

        static void setup_globals(crt_heap const& heap);

    private:
        [[nodiscard]] std::wstring get_filename() const;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info crt_mem_block_header_symbol_type;
            size_t crt_mem_block_header_length{};
            stream_utils::symbol_type_and_base_type_field_offset crt_mem_block_header_line_number_field_data;
            stream_utils::symbol_type_and_base_type_field_offset crt_mem_block_header_block_use_field_data;
            stream_utils::symbol_type_and_base_type_field_offset crt_mem_block_header_request_number_field_data;
            stream_utils::symbol_type_and_base_type_field_offset crt_mem_block_header_data_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset crt_mem_block_header_file_name_field_data;
        };

        cache_data const& cache_data_;
        crt_heap const& heap_;
        uint64_t const entry_address_;
        std::wstring const file_name_;
    };
}
