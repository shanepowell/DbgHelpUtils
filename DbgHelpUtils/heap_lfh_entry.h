﻿#pragma once
#include <cstdint>
#include <optional>

#include "size_units.h"
#include "tagged_bool.h"

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
    class heap_lfh_context;

    using allocated_t = tagged_bool<struct allocated_type>;
    using has_unused_bytes_t = tagged_bool<struct has_unused_bytes_type>;

    class heap_lfh_entry
    {
    public:
        heap_lfh_entry(heap_lfh_context const& heap, uint64_t heap_lfh_entry_address, size_units::base_16::bytes block_size, allocated_t allocated, has_unused_bytes_t has_unused_bytes);

        [[nodiscard]] heap_lfh_context const& heap() const { return *heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_lfh_entry_address() const { return heap_lfh_entry_address_; }

        [[nodiscard]] size_units::base_16::bytes block_size() const { return block_size_; }
        [[nodiscard]] bool allocated() const { return static_cast<bool>(allocated_); }
        [[nodiscard]] bool has_unused_bytes() const { return static_cast<bool>(has_unused_bytes_); }
        [[nodiscard]] size_units::base_16::bytes unused_bytes() const { return unused_bytes_; }
        [[nodiscard]] std::optional<uint64_t> const& front_padding_size() const { return front_padding_size_; }

        [[nodiscard]] uint64_t block_address() const { return heap_lfh_entry_address(); }
        [[nodiscard]] uint64_t user_address() const;
        [[nodiscard]] size_units::base_16::bytes user_requested_size() const;

        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }
        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }

    private:
        [[nodiscard]] size_units::base_16::bytes get_unused_bytes() const;
        [[nodiscard]] uint64_t get_ust_address() const;
        [[nodiscard]] std::vector<uint64_t> get_allocation_stack_trace() const;
        [[nodiscard]] std::optional<uint64_t> get_front_padding_size() const;

    private:
        heap_lfh_context const* heap_;
        uint64_t heap_lfh_entry_address_;
        size_units::base_16::bytes block_size_;
        allocated_t allocated_;
        has_unused_bytes_t has_unused_bytes_;
        size_units::base_16::bytes unused_bytes_;
        std::optional<uint64_t> front_padding_size_;
        uint64_t ust_address_;
        std::vector<uint64_t> allocation_stack_trace_{};
    };
}

