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
    class ust_address_stack_trace;
    class nt_heap;

    class heap_entry
    {
    public:
        struct UnknownSizeType
        {
        };

        struct LfhEntryType
        {
        };

        struct VirtualAllocType
        {
        };

        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, std::shared_ptr<uint8_t[]> buffer);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, std::shared_ptr<uint8_t[]> buffer, uint16_t block_size, LfhEntryType);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t end_address, std::shared_ptr<uint8_t[]> buffer, uint64_t size, uint16_t unused_bytes, VirtualAllocType);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, std::shared_ptr<uint8_t[]> buffer, size_units::base_16::bytes previous_size);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t uncommitted_size);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t unknown_size, UnknownSizeType);

        [[nodiscard]] nt_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] uint64_t address() const { return heap_entry_address_; }
        [[nodiscard]] uint64_t user_address() const { return user_address_; }
        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }
        [[nodiscard]] uint64_t block_address() const { return heap_entry_address_ + cache_data_.heap_entry_length; }
        [[nodiscard]] uint64_t block_size() const { return size().count() - cache_data_.heap_entry_length; }

        [[nodiscard]] uint8_t flags() const { return flags_; }
        [[nodiscard]] size_units::base_16::bytes size() const { return size_; }
        [[nodiscard]] size_units::base_16::bytes previous_size() const { return previous_size_; }
        [[nodiscard]] size_units::base_16::bytes user_requested_size() const { return requested_size_; }
        [[nodiscard]] size_units::base_16::bytes end_unused_bytes() const { return end_unused_bytes_; }
        [[nodiscard]] size_units::base_16::bytes unused_bytes() const { return unused_bytes_; }

        [[nodiscard]] uint8_t segment_offset() const { return segment_offset_; }
        [[nodiscard]] uint8_t small_tag_index() const { return small_tag_index_; }
        [[nodiscard]] bool is_uncommitted() const { return buffer_ == nullptr && flags_ == 0; }
        [[nodiscard]] bool is_unknown() const { return buffer_ == nullptr && flags_ == FlagBusy; }
        [[nodiscard]] bool is_virtual_alloc() const { return is_virtual_alloc_ || buffer_ == nullptr && flags_ == FlagBusy; }
        [[nodiscard]] bool is_front_padded() const { return size_units::base_16::bytes{unused_bytes_raw()} > size_; }
        [[nodiscard]] bool is_lfh_entry() const { return (raw_unused_bytes_ & FlagLFHMarker) == FlagLFHMarker; }
        [[nodiscard]] bool is_lfh_busy() const;
        [[nodiscard]] bool is_busy() const { return !is_uncommitted() && !is_unknown() && (is_lfh_entry() ? is_lfh_busy() : (flags() & FlagBusy) == FlagBusy); }

        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }
        
        static uint8_t constexpr FlagBusy = 0x01;
        static uint8_t constexpr FlagExtraPresent = 0x02;
        static uint8_t constexpr FlagFillPattern = 0x04;
        static uint8_t constexpr FlagVirtualAlloc = 0x08;
        static uint8_t constexpr FlagLastEntry = 0x10;
        static uint8_t constexpr FlagUserMask = 0xE0;
        static uint8_t constexpr FlagUserBitOffset = 5;

        static uint8_t constexpr FlagLFHMarker = 0x80;
        static uint8_t constexpr LfhFlagUstBusy = 0xc2;

        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_.heap_entry_symbol_type; }

        static void setup_globals(nt_heap const& heap);
        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] uint8_t unused_bytes_raw() const
        {
            return raw_unused_bytes_ & ~FlagLFHMarker;
        }

        [[nodiscard]] bool is_heap_allocation() const;
        [[nodiscard]] bool get_is_valid(size_units::base_16::bytes previous_size) const;
        [[nodiscard]] uint8_t get_flags() const;
        [[nodiscard]] uint64_t get_size() const;
        [[nodiscard]] uint64_t get_previous_size() const;
        [[nodiscard]] uint8_t get_segment_offset() const;
        [[nodiscard]] uint8_t get_small_tag_index() const;
        [[nodiscard]] uint8_t get_raw_unused_bytes() const;
        [[nodiscard]] size_units::base_16::bytes get_unused_bytes() const;
        [[nodiscard]] size_units::base_16::bytes get_requested_size() const;
        [[nodiscard]] uint64_t get_user_address() const;
        [[nodiscard]] size_units::base_16::bytes get_end_unused_bytes() const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] uint64_t get_ust_user_address() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;
        [[nodiscard]] static size_units::base_16::bytes get_virtual_alloc_requested_size(uint64_t size, uint16_t unused_bytes);
        [[nodiscard]] size_units::base_16::bytes get_virtual_alloc_end_unused_bytes(uint64_t end_address) const;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_entry_symbol_type;
            size_t heap_entry_length{};
            stream_utils::symbol_type_and_base_type_field_offset heap_entry_flags_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_entry_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_entry_previous_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_entry_segment_offset_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_entry_small_tag_index_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_entry_unused_bytes_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_entry_unused_bytes_length_field_data;
        };

        cache_data const& cache_data_;
        nt_heap const& heap_;
        uint64_t const heap_entry_address_;
        std::shared_ptr<uint8_t[]> buffer_;
        uint8_t const flags_{0};
        size_units::base_16::bytes const size_;
        size_units::base_16::bytes const previous_size_{0};
        uint8_t const small_tag_index_{0};
        uint8_t const segment_offset_{0};
        uint8_t const raw_unused_bytes_{0};
        uint64_t const ust_user_address_{0};
        size_units::base_16::bytes unused_bytes_{0};
        size_units::base_16::bytes requested_size_{0};
        uint64_t const user_address_{0};
        size_units::base_16::bytes end_unused_bytes_{0};
        uint64_t const ust_address_{0};
        bool const is_valid_{true};
        bool const is_virtual_alloc_{false};
        std::vector<uint64_t> const allocation_stack_trace_{};
    };

}
