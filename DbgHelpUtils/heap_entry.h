#pragma once
#include <cstdint>

#include "size_units.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::heap
{
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

        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, std::shared_ptr<char[]> buffer);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, std::shared_ptr<char[]> buffer, uint16_t block_size, LfhEntryType);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t end_address, std::shared_ptr<char[]> buffer, uint64_t size, uint16_t unused_bytes, VirtualAllocType);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, std::shared_ptr<char[]> buffer, size_units::base_10::bytes previous_size);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t uncommitted_size);
        heap_entry(nt_heap const& heap, uint64_t heap_entry_address, uint64_t unknown_size, UnknownSizeType);

        [[nodiscard]] nt_heap const& heap() const { return heap_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] uint64_t address() const { return heap_entry_address_; }
        [[nodiscard]] uint64_t user_address() const { return user_address_; }
        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }

        [[nodiscard]] uint8_t flags() const { return flags_; }
        [[nodiscard]] size_units::base_10::bytes size() const { return size_; }
        [[nodiscard]] size_units::base_10::bytes previous_size() const { return previous_size_; }
        [[nodiscard]] size_units::base_10::bytes requested_size() const { return requested_size_; }
        [[nodiscard]] size_units::base_10::bytes end_unused_bytes() const { return end_unused_bytes_; }
        [[nodiscard]] size_units::base_10::bytes unused_bytes() const { return unused_bytes_; }

        [[nodiscard]] uint8_t segment_offset() const { return segment_offset_; }
        [[nodiscard]] bool is_uncommitted() const { return buffer_ == nullptr && flags_ == 0; }
        [[nodiscard]] bool is_unknown() const { return buffer_ == nullptr && flags_ == FlagBusy; }
        [[nodiscard]] bool is_virtual_alloc() const { return is_virtual_alloc_ || buffer_ == nullptr && flags_ == FlagBusy; }
        [[nodiscard]] bool is_front_padded() const { return size_units::base_10::bytes{unused_bytes_raw()} > size_; }
        [[nodiscard]] bool is_lfh_entry() const { return (raw_unused_bytes_ & FlagLFHMarker) == FlagLFHMarker; }
        [[nodiscard]] bool is_lfh_busy() const;
        [[nodiscard]] bool is_busy() const { return !is_uncommitted() && !is_unknown() && (is_lfh_entry() ? is_lfh_busy() : (flags() & FlagBusy) == FlagBusy); }
        [[nodiscard]] bool is_heap_allocation() const;

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

    private:
        [[nodiscard]] uint8_t unused_bytes_raw() const
        {
            return raw_unused_bytes_ & ~FlagLFHMarker;
        }

        template <typename T>
        [[nodiscard]] T get_field_value(std::wstring const& field_name) const;

        [[nodiscard]] bool get_is_valid(size_units::base_10::bytes previous_size) const;
        [[nodiscard]] uint8_t get_flags() const;
        [[nodiscard]] uint64_t get_size() const;
        [[nodiscard]] uint64_t get_previous_size() const;
        [[nodiscard]] uint8_t get_segment_offset() const;
        [[nodiscard]] uint8_t get_raw_unused_bytes() const;
        [[nodiscard]] bool is_valid_ust_area() const;
        [[nodiscard]] size_units::base_10::bytes get_unused_bytes() const;
        [[nodiscard]] size_units::base_10::bytes get_requested_size() const;
        [[nodiscard]] uint64_t get_user_address() const;
        [[nodiscard]] size_units::base_10::bytes get_end_unused_bytes() const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] uint64_t get_ust_extra_offset() const;
        [[nodiscard]] uint64_t get_ust_data_size() const;
        [[nodiscard]] uint64_t get_ust_address_depth_offset() const;
        [[nodiscard]] uint64_t get_ust_address_stack_offset() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;
        [[nodiscard]] static size_units::base_10::bytes get_virtual_alloc_requested_size(uint64_t size, uint16_t unused_bytes);
        [[nodiscard]] size_units::base_10::bytes get_virtual_alloc_end_unused_bytes(uint64_t end_address) const;

        [[nodiscard]] dbg_help::symbol_type_info get_heap_entry_symbol_type() const;
        [[nodiscard]] uint64_t get_heap_entry_length() const;
        [[nodiscard]] uint64_t get_small_tag_index_offset() const;

    private:
        nt_heap const& heap_;
        uint64_t const heap_entry_address_;
        dbg_help::symbol_type_info const heap_entry_symbol_type_{get_heap_entry_symbol_type()};
        uint64_t const heap_entry_length_{get_heap_entry_length()};
        std::shared_ptr<char[]> buffer_;
        uint64_t const small_tag_index_offset_{0};
        uint8_t const flags_{0};
        size_units::base_10::bytes const size_;
        size_units::base_10::bytes const previous_size_{0};
        uint8_t const segment_offset_{0};
        uint8_t const raw_unused_bytes_{0};
        bool const is_valid_ust_area_{false};
        size_units::base_10::bytes unused_bytes_{0};
        size_units::base_10::bytes requested_size_{0};
        uint64_t const user_address_{0};
        size_units::base_10::bytes end_unused_bytes_{0};
        uint64_t const ust_address_{0};
        bool const is_valid_{true};
        bool const is_virtual_alloc_{false};
        std::vector<uint64_t> const allocation_stack_trace_{};
    };

}
