#pragma once
#include <cstdint>

#include "memory_range.h"
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

        enum class user_requested_size_type : uint8_t
        {
            none,
            raw_gt_size,
            raw_gt_size_gt_size,
            raw_lte_size,
            bad_unused_size,
        };

        enum class unused_bytes_type : uint8_t
        {
            none,
            unused_bytes_gt_data_area_size,
            unused_bytes_gt_data_area_size_plus_offset,
            unused_bytes_gt_data_area_size_offset_diff,
            unused_bytes_gt_data_area_size_gt_data_area_size,
            unused_bytes_lte_data_area_size_minus_offset,
            unused_bytes_lte_data_area_size_plus_offset,
            size_gte_ust_unused_bytes_gte_data_area_size,
            unused_bytes_ust_header_gap,
            unused_bytes_eod_size_offset,
        };

        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, std::shared_ptr<uint8_t[]> buffer);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, std::shared_ptr<uint8_t[]> buffer, uint16_t block_size, LfhEntryType);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t end_address, std::shared_ptr<uint8_t[]> buffer, uint64_t size, uint16_t unused_bytes, VirtualAllocType);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, std::shared_ptr<uint8_t[]> buffer, size_units::base_16::bytes previous_size);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t uncommitted_size);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t uncommitted_size, VirtualAllocType);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t unknown_size, UnknownSizeType);

        [[nodiscard]] nt_heap const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] uint64_t address() const { return heap_entry_address_; }
        [[nodiscard]] uint64_t user_address() const { return user_address_; }
        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }
        [[nodiscard]] uint64_t block_address() const { return address() + cache_data_->heap_entry_length; }
        [[nodiscard]] uint64_t block_size() const { return size().count() - cache_data_->heap_entry_length; }

        [[nodiscard]] uint8_t flags() const { return flags_; }
        [[nodiscard]] size_units::base_16::bytes size() const { return size_; }
        [[nodiscard]] size_units::base_16::bytes previous_size() const { return previous_size_; }
        [[nodiscard]] size_units::base_16::bytes user_requested_size() const { return requested_size_; }
        [[nodiscard]] size_units::base_16::bytes end_unused_bytes() const { return end_unused_bytes_; }
        [[nodiscard]] size_units::base_16::bytes unused_bytes() const { return unused_bytes_; }
        [[nodiscard]] size_units::base_16::bytes ust_unused_bytes_raw() const;
        [[nodiscard]] size_units::base_16::bytes data_area_max_size() const;
        [[nodiscard]] std::optional<size_units::base_16::bytes> const& eod() const { return eod_; }
        [[nodiscard]] uint64_t ust_header_address() const { return ust_header_address_; }
        [[nodiscard]] std::optional<size_units::base_16::bytes> const& ust_end_gap_length() const { return ust_end_gap_length_; }

        [[nodiscard]] unused_bytes_type unused_bytes_method() const { return unused_bytes_method_; }
        [[nodiscard]] user_requested_size_type requested_size_method() const { return requested_size_method_; }

        [[nodiscard]] uint8_t unused_bytes_raw() const
        {
            return raw_unused_bytes_ & ~FlagLFHMarker;
        }

        [[nodiscard]] size_units::base_16::bytes unused_bytes_raw_data() const { return size_units::base_16::bytes{raw_unused_bytes_}; }

        [[nodiscard]] uint8_t segment_offset() const { return segment_offset_; }
        [[nodiscard]] uint8_t small_tag_index() const { return small_tag_index_; }
        [[nodiscard]] bool is_uncommitted() const { return buffer_ == nullptr && flags_ == 0; }
        [[nodiscard]] bool is_unknown() const { return buffer_ == nullptr && flags_ == FlagBusy; }
        [[nodiscard]] bool is_virtual_alloc() const { return is_virtual_alloc_ || buffer_ == nullptr && flags_ == FlagBusy; }
        [[nodiscard]] bool has_front_padding() const { return size_units::base_16::bytes{unused_bytes_raw()} > size_; }
        [[nodiscard]] bool is_lfh_entry() const { return (raw_unused_bytes_ & FlagLFHMarker) == FlagLFHMarker; }
        [[nodiscard]] bool is_lfh_busy() const;
        [[nodiscard]] bool is_busy() const { return !is_uncommitted() && !is_unknown() && (is_lfh_entry() ? is_lfh_busy() : (flags() & FlagBusy) == FlagBusy); }
        [[nodiscard]] uint16_t lfh_index() const { return lfh_index_; }

        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }
        [[nodiscard]] memory_range memory_range() const { return dlg_help_utils::memory_range{ address(), address() + static_cast<uint64_t>(size().count()) }; }

        static uint8_t constexpr FlagBusy = 0x01;
        static uint8_t constexpr FlagExtraPresent = 0x02;
        static uint8_t constexpr FlagFillPattern = 0x04;
        static uint8_t constexpr FlagVirtualAlloc = 0x08;
        static uint8_t constexpr FlagLastEntry = 0x10;
        static uint8_t constexpr FlagUserMask = 0xE0;
        static uint8_t constexpr FlagUserBitOffset = 5;

        static uint8_t constexpr FlagLFHMarker = 0x80;
        static uint8_t constexpr LfhFlagUstBusy = 0xc2;

        [[nodiscard]] uint64_t symbol_address() const { return address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_entry_symbol_type; }

        static void setup_globals(nt_heap const& heap);
        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] bool is_heap_allocation() const;
        [[nodiscard]] bool get_is_valid(size_units::base_16::bytes previous_size) const;
        [[nodiscard]] uint8_t get_flags() const;
        [[nodiscard]] uint64_t get_size() const;
        [[nodiscard]] uint64_t get_previous_size() const;
        [[nodiscard]] uint16_t get_lfh_index() const;
        [[nodiscard]] uint8_t get_segment_offset() const;
        [[nodiscard]] uint8_t get_small_tag_index() const;
        [[nodiscard]] uint8_t get_raw_unused_bytes() const;
        [[nodiscard]] size_units::base_16::bytes get_unused_bytes(unused_bytes_type& unused_bytes_method, std::optional<size_units::base_16::bytes>& eod, std::optional<size_units::base_16::bytes>& ust_end_gap_length) const;
        [[nodiscard]] size_units::base_16::bytes get_requested_size(user_requested_size_type& requested_size_method) const;
        [[nodiscard]] uint64_t get_user_address() const;
        [[nodiscard]] size_units::base_16::bytes get_end_unused_bytes() const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] uint64_t get_ust_header_address() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;
        [[nodiscard]] static size_units::base_16::bytes get_virtual_alloc_requested_size(uint64_t size, uint16_t unused_bytes);
        [[nodiscard]] size_units::base_16::bytes get_virtual_alloc_end_unused_bytes(uint64_t end_address) const;
        [[nodiscard]] size_units::base_16::bytes front_padding() const { return size_units::base_16::bytes{has_front_padding() ? cache_data_->heap_entry_length : 0}; }

        void validate_entry() const;

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
            uint64_t previous_block_private_data_size{};
        };

        cache_data const* cache_data_;
        nt_heap const* heap_;
        uint64_t heap_entry_address_;
        std::shared_ptr<uint8_t[]> buffer_;
        uint8_t flags_{0};
        size_units::base_16::bytes size_;
        size_units::base_16::bytes previous_size_{0};
        uint16_t lfh_index_{0};
        uint8_t small_tag_index_{0};
        uint8_t segment_offset_{0};
        uint8_t raw_unused_bytes_{0};
        uint64_t ust_header_address_{0};
        uint64_t user_address_{0};
        unused_bytes_type unused_bytes_method_{unused_bytes_type::none};
        std::optional<size_units::base_16::bytes> eod_;
        std::optional<size_units::base_16::bytes> ust_end_gap_length_{};
        size_units::base_16::bytes unused_bytes_{0};
        user_requested_size_type requested_size_method_{user_requested_size_type::none};
        size_units::base_16::bytes requested_size_{0};
        size_units::base_16::bytes end_unused_bytes_{0};
        uint64_t ust_address_{0};
        bool is_valid_{true};
        bool is_virtual_alloc_{false};
        std::vector<uint64_t> allocation_stack_trace_{};
    };

}
