#pragma once
#include <cstdint>
#include <vector>

#include "size_units.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::heap
{
    class dph_heap;

    class dph_entry
    {
    public:
        static constexpr uint32_t StampAllocNormalPageMode = 0xABCDAAAA;
        static constexpr uint32_t StampFreeNormalPageMode = 0xABCDAAA9;
        static constexpr uint32_t StampAllocFullPageMode = 0xABCDBBBB;
        static constexpr uint32_t StampFreeFullPageMode = 0xABCDBBBA;


        dph_entry(dph_heap const& heap, uint64_t entry_address);

        [[nodiscard]] uint64_t entry_address() const { return entry_address_; }
        [[nodiscard]] dph_heap const& heap() const { return heap_; }

        [[nodiscard]] uint64_t virtual_block_address() const { return virtual_block_address_; }
        [[nodiscard]] size_units::base_10::bytes virtual_block_size() const { return virtual_block_size_; }
        [[nodiscard]] uint64_t user_address() const { return user_address_; }
        [[nodiscard]] size_units::base_10::bytes user_requested_size() const { return user_requested_size_; }
        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }
        [[nodiscard]] bool is_allocated() const { return allocated_; }
        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }

        [[nodiscard]] uint64_t next_alloc_address() const { return next_alloc_address_; }

    private:
        [[nodiscard]] uint64_t get_virtual_block_address() const;
        [[nodiscard]] size_units::base_10::bytes get_virtual_block_size() const;
        [[nodiscard]] uint64_t get_user_address() const;
        [[nodiscard]] size_units::base_10::bytes get_user_requested_size() const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] uint64_t get_next_alloc_address() const;
        [[nodiscard]] bool get_is_allocated() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;

        template <typename T>
        [[nodiscard]] T get_field_value(std::wstring const& field_name) const;

        [[nodiscard]] uint64_t get_machine_size_field_value(std::wstring const& field_name) const;
        [[nodiscard]] uint64_t get_dph_block_information_symbol_length() const;

    private:
        dph_heap const& heap_;
        dbg_help::symbol_type_info const dph_heap_block_symbol_type_;
        dbg_help::symbol_type_info const dph_block_information_symbol_type_;
        uint64_t const dph_block_information_symbol_length_;
        uint64_t const entry_address_;
        uint64_t const virtual_block_address_;
        size_units::base_10::bytes const virtual_block_size_;
        uint64_t const user_address_;
        size_units::base_10::bytes const user_requested_size_;
        uint64_t const ust_address_;
        uint64_t next_alloc_address_;
        bool const allocated_;
        std::vector<uint64_t> const allocation_stack_trace_{};
    };

}
