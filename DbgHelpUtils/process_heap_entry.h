#pragma once
#include <cstdint>

#include "heap_match_utils.h"
#include "memory_range.h"
#include "mini_dump_memory_stream.h"
#include "size_units.h"

namespace dlg_help_utils
{
    struct memory_range;
}

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::heap
{
    class crt_entry;
    class large_alloc_entry;
    class heap_vs_entry;
    class page_range_descriptor;
    class dph_entry;
    class heap_entry;
    class heap_lfh_entry;

    class process_heap_entry
    {
    public:
        process_heap_entry(dph_entry const& entry);
        process_heap_entry(heap_entry const& entry);
        process_heap_entry(heap_lfh_entry const& entry);
        process_heap_entry(page_range_descriptor const& entry);
        process_heap_entry(heap_vs_entry const& entry);
        process_heap_entry(large_alloc_entry const& entry);
        process_heap_entry(process::process_environment_block const& peb, memory_range const& entry);

        process_heap_entry(dph_entry const& entry, crt_entry const& crt_block);
        process_heap_entry(heap_entry const& entry, crt_entry const& crt_block);
        process_heap_entry(heap_lfh_entry const& entry, crt_entry const& crt_block);
        process_heap_entry(page_range_descriptor const& entry, crt_entry const& crt_block);
        process_heap_entry(heap_vs_entry const& entry, crt_entry const& crt_block);
        process_heap_entry(large_alloc_entry const& entry, crt_entry const& crt_block);

        [[nodiscard]] uint64_t user_address() const;
        [[nodiscard]] size_units::base_16::bytes user_requested_size() const;
        [[nodiscard]] size_units::base_16::bytes overhead_size() const { return overhead_size_; }
        [[nodiscard]] std::wstring const& filename() const { return file_name_; }
        [[nodiscard]] uint32_t line_number() const { return line_number_; }
        [[nodiscard]] bool has_request_number() const { return has_request_number_; }
        [[nodiscard]] uint32_t request_number() const { return request_number_; }
        [[nodiscard]] memory_range user_memory_range() const { return memory_range{.start_range= user_address(), .end_range= user_address() + static_cast<uint64_t>(user_requested_size().count())}; }

        [[nodiscard]] uint64_t ust_address() const { return ust_address_; }
        [[nodiscard]] std::vector<uint64_t> const& allocation_stack_trace() const { return allocation_stack_trace_; }

        [[nodiscard]] mini_dump_memory_stream user_data(uint64_t range_address, size_units::base_16::bytes range_size) const;
        [[nodiscard]] mini_dump_memory_stream all_user_data() const;

        [[nodiscard]] bool contains_address_range(uint64_t address, size_units::base_16::bytes size) const;
        [[nodiscard]] bool contains_metadata_address_range(uint64_t address, size_units::base_16::bytes size) const;

        [[nodiscard]] bool operator==(process_heap_entry const& rhs) const
        {
            return has_request_number() == rhs.has_request_number() && request_number() == rhs.request_number() && user_address() == rhs.user_address() && user_requested_size() == rhs.user_requested_size();
        }

        [[nodiscard]] block_range_match_result match_range(uint64_t range_address, size_units::base_16::bytes range_size) const;

        [[nodiscard]] bool is_crt_heap_entry() const { return is_crt_heap_entry_; }
        [[nodiscard]] heap_node_type get_heap_entry_type() const { return heap_entry_type_; }

    private:
        [[nodiscard]] uint64_t get_nt_heap_entry_check_block_start(heap_entry const& entry) const;
        [[nodiscard]] uint64_t get_nt_heap_entry_check_block_size(heap_entry const& entry) const;
        [[nodiscard]] static heap_node_type get_heap_entry_node_type(heap_entry const& entry);
        [[nodiscard]] static heap_node_type get_dph_entry_type(dph_entry const& entry);
        void validate_entry() const;

    private:
        process::process_environment_block const* peb_;
        uint64_t user_address_;
        size_units::base_16::bytes user_size_;
        uint64_t ust_address_;
        std::vector<uint64_t> allocation_stack_trace_;
        std::wstring file_name_;
        uint32_t line_number_{0};
        bool has_request_number_{false};
        uint32_t request_number_{0};
        uint64_t check_block_start_address_;
        uint64_t check_block_end_address_;
        uint64_t check_metadata_start_address_;
        uint64_t check_metadata_end_address_;
        size_units::base_16::bytes overhead_size_;
        bool is_crt_heap_entry_{false};
        heap_node_type heap_entry_type_;
    };
}
