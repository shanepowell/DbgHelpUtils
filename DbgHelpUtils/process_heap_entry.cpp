#include "process_heap_entry.h"

#include "crt_entry.h"
#include "dph_entry.h"
#include "heap_entry.h"
#include "heap_lfh_entry.h"
#include "heap_node_type.h"
#include "heap_vs_entry.h"
#include "large_alloc_entry.h"
#include "memory_range.h"
#include "page_range_descriptor.h"
#include "process_environment_block.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::heap
{
    process_heap_entry::process_heap_entry(dph_entry const& entry)
    : peb_{&entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , check_block_start_address_{entry.virtual_block_address()}
    , check_block_end_address_{check_block_start_address_ + entry.virtual_block_size().count()}
    , check_metadata_start_address_{entry.entry_address()}
    , check_metadata_end_address_{check_block_start_address_}
    , overhead_size_{entry.virtual_block_size().count() - user_size_.count()}
    , heap_entry_type_{(get_dph_entry_type(entry))}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(heap_entry const& entry)
    : peb_{&entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , check_block_start_address_{get_nt_heap_entry_check_block_start(entry)}
    , check_block_end_address_{check_block_start_address_ + get_nt_heap_entry_check_block_size(entry)}
    , check_metadata_start_address_{entry.address()}
    , check_metadata_end_address_{check_block_start_address_}
    , overhead_size_{entry.unused_bytes()}
    , heap_entry_type_{(get_heap_entry_node_type(entry))}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(heap_lfh_entry const& entry)
    : peb_{&entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , check_block_start_address_{entry.user_address()}
    , check_block_end_address_{check_block_start_address_ + entry.user_requested_size().count()}
    , check_metadata_start_address_{entry.block_address()}
    , check_metadata_end_address_{check_metadata_start_address_ + entry.block_size().count()}
    , overhead_size_{entry.block_size().count() - user_size_.count()}
    , heap_entry_type_{heap_node_type::segment_lfh_entry}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(page_range_descriptor const& entry)
    : peb_{&entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , check_block_start_address_{entry.block_address()}
    , check_block_end_address_{check_block_start_address_ + entry.block_size().count()}
    , check_metadata_start_address_{entry.page_range_descriptor_address()}
    , check_metadata_end_address_{check_metadata_start_address_ + entry.symbol_type().length().value_or(1)}
    , overhead_size_{entry.block_size().count() - user_size_.count()}
    , heap_entry_type_{heap_node_type::segment_backend_entry}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(heap_vs_entry const& entry)
    : peb_{&entry.peb()}
    , user_address_{entry.uncommitted_range() ? entry.block_address() : entry.user_address()}
    , user_size_{entry.uncommitted_range() ? size_units::base_16::bytes{entry.block_size()} : entry.user_requested_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , check_block_start_address_{entry.block_address()}
    , check_block_end_address_{check_block_start_address_ + entry.block_size()}
    , check_metadata_start_address_{entry.heap_vs_entry_address()}
    , check_metadata_end_address_{entry.block_address()}
    , overhead_size_{entry.block_size() - user_size_.count()}
    , heap_entry_type_{heap_node_type::segment_entry}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(large_alloc_entry const& entry)
    : peb_{&entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , check_block_start_address_{entry.user_address()}
    , check_block_end_address_{check_block_start_address_ + entry.user_requested_size().count()}
    , check_metadata_start_address_{entry.large_alloc_entry_address()}
    , check_metadata_end_address_{check_metadata_start_address_ + entry.symbol_type().length().value_or(1)}
    , overhead_size_{entry.block_size() - user_size_.count()}
    , heap_entry_type_{heap_node_type::segment_large_entry}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(process::process_environment_block const& peb, memory_range const& entry)
    : peb_{&peb}
    , user_address_{entry.start_range}
    , user_size_{entry.end_range - entry.start_range}
    , ust_address_{0}
    , check_block_start_address_{entry.start_range}
    , check_block_end_address_{entry.end_range}
    , check_metadata_start_address_{entry.start_range}
    , check_metadata_end_address_{entry.end_range}
    , overhead_size_{0}
    , heap_entry_type_{heap_node_type::memory_range}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(dph_entry const& entry, crt_entry const& crt_block)
    : peb_{&entry.peb()}
    , user_address_{crt_block.user_address()}
    , user_size_{crt_block.data_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , file_name_{crt_block.filename()}
    , line_number_{crt_block.line_number()}
    , has_request_number_{true}
    , request_number_{crt_block.request_number()}
    , check_block_start_address_{entry.virtual_block_address()}
    , check_block_end_address_{check_block_start_address_ + entry.virtual_block_size().count()}
    , check_metadata_start_address_{entry.entry_address()}
    , check_metadata_end_address_{check_metadata_start_address_ + entry.symbol_type().length().value_or(1)}
    , overhead_size_{entry.virtual_block_size().count() - user_size_.count()}
    , is_crt_heap_entry_{true}
    , heap_entry_type_{(get_dph_entry_type(entry))}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(heap_entry const& entry, crt_entry const& crt_block)
    : peb_{&entry.peb()}
    , user_address_{crt_block.user_address()}
    , user_size_{crt_block.data_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , file_name_{crt_block.filename()}
    , line_number_{crt_block.line_number()}
    , has_request_number_{true}
    , request_number_{crt_block.request_number()}
    , check_block_start_address_{get_nt_heap_entry_check_block_start(entry)}
    , check_block_end_address_{check_block_start_address_ + get_nt_heap_entry_check_block_size(entry)}
    , check_metadata_start_address_{entry.address()}
    , check_metadata_end_address_{check_metadata_start_address_ + entry.symbol_type().length().value_or(1)}
    , overhead_size_{entry.unused_bytes()}
    , is_crt_heap_entry_{true}
    , heap_entry_type_{(get_heap_entry_node_type(entry))}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(heap_lfh_entry const& entry, crt_entry const& crt_block)
    : peb_{&entry.peb()}
    , user_address_{crt_block.user_address()}
    , user_size_{crt_block.data_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , file_name_{crt_block.filename()}
    , line_number_{crt_block.line_number()}
    , has_request_number_{true}
    , request_number_{crt_block.request_number()}
    , check_block_start_address_{entry.block_address()}
    , check_block_end_address_{check_block_start_address_ + entry.block_size().count()}
    , check_metadata_start_address_{check_block_start_address_}
    , check_metadata_end_address_{check_block_end_address_}
    , overhead_size_{entry.block_size().count() - user_size_.count()}
    , is_crt_heap_entry_{true}
    , heap_entry_type_{heap_node_type::segment_lfh_entry}
    {
    }

    process_heap_entry::process_heap_entry(page_range_descriptor const& entry, crt_entry const& crt_block)
    : peb_{&entry.peb()}
    , user_address_{crt_block.user_address()}
    , user_size_{crt_block.data_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , file_name_{crt_block.filename()}
    , line_number_{crt_block.line_number()}
    , has_request_number_{true}
    , request_number_{crt_block.request_number()}
    , check_block_start_address_{entry.block_address()}
    , check_block_end_address_{check_block_start_address_ + entry.block_size().count()}
    , check_metadata_start_address_{entry.page_range_descriptor_address()}
    , check_metadata_end_address_{check_metadata_start_address_ + entry.symbol_type().length().value_or(1)}
    , overhead_size_{entry.block_size().count() - user_size_.count()}
    , is_crt_heap_entry_{true}
    , heap_entry_type_{heap_node_type::segment_backend_entry}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(heap_vs_entry const& entry, crt_entry const& crt_block)
    : peb_{&entry.peb()}
    , user_address_{crt_block.user_address()}
    , user_size_{crt_block.data_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , file_name_{crt_block.filename()}
    , line_number_{crt_block.line_number()}
    , has_request_number_{true}
    , request_number_{crt_block.request_number()}
    , check_block_start_address_{entry.block_address()}
    , check_block_end_address_{check_block_start_address_ + entry.block_size()}
    , check_metadata_start_address_{entry.heap_vs_entry_address()}
    , check_metadata_end_address_{entry.block_address()}
    , overhead_size_{entry.block_size() - user_size_.count()}
    , is_crt_heap_entry_{true}
    , heap_entry_type_{heap_node_type::segment_entry}
    {
        validate_entry();
    }

    process_heap_entry::process_heap_entry(large_alloc_entry const& entry, crt_entry const& crt_block)
    : peb_{&entry.peb()}
    , user_address_{crt_block.user_address()}
    , user_size_{crt_block.data_size()}
    , ust_address_{entry.ust_address()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    , file_name_{crt_block.filename()}
    , line_number_{crt_block.line_number()}
    , has_request_number_{true}
    , request_number_{crt_block.request_number()}
    , check_block_start_address_{entry.block_address()}
    , check_block_end_address_{check_block_start_address_ + entry.block_size()}
    , check_metadata_start_address_{entry.large_alloc_entry_address()}
    , check_metadata_end_address_{check_metadata_start_address_ + entry.symbol_type().length().value_or(1)}
    , overhead_size_{entry.block_size() - user_size_.count()}
    , is_crt_heap_entry_{true}
    , heap_entry_type_{heap_node_type::segment_large_entry}
    {
        validate_entry();
    }

    uint64_t process_heap_entry::user_address() const
    {
        if(user_address_ == 0)
        {
            return check_block_start_address_;
        }
        return user_address_;
    }

    size_units::base_16::bytes process_heap_entry::user_requested_size() const
    {
        if(user_address_ == 0)
        {
            return  size_units::base_16::bytes{check_block_end_address_ - check_block_start_address_};
        }
        return user_size_;
    }

    mini_dump_memory_stream process_heap_entry::user_data(uint64_t const range_address, size_units::base_16::bytes const range_size) const
    {
        switch(match_range(range_address, range_size))
        {
        case block_range_match_result::block_match:
        case block_range_match_result::block_contains:
            return peb_->walker().get_process_memory_stream(range_address, range_size.count());

        case block_range_match_result::user_contains_block:
            return peb_->walker().get_process_memory_stream(user_address(), user_requested_size().count());

        case block_range_match_result::block_partially_contains:
            {
                auto const start_range = std::max(range_address, user_address());
                auto const end_range = std::min(range_address + range_size.count(), user_address() + user_requested_size().count());
                return peb_->walker().get_process_memory_stream(start_range, end_range - start_range);
            }

        case block_range_match_result::block_no_match:
            break;
        }

        using namespace size_units::base_16;
        throw exceptions::wide_runtime_error{std::format(L"not a range user data range [{0} - {1}]", range_address, to_wstring(range_size))};
    }

    mini_dump_memory_stream process_heap_entry::all_user_data() const
    {
        return peb_->walker().get_process_memory_stream(user_address(), user_requested_size().count());
    }

    bool process_heap_entry::contains_address_range(uint64_t const address, size_units::base_16::bytes const size) const
    {
        return address >= check_block_start_address_ && address + size.count() <= check_block_end_address_;
    }

    bool process_heap_entry::contains_metadata_address_range(uint64_t const address, size_units::base_16::bytes const size) const
    {
        return address >= check_metadata_start_address_ && address + size.count() <= check_metadata_end_address_;
    }

    block_range_match_result process_heap_entry::match_range(uint64_t const range_address, size_units::base_16::bytes const range_size) const
    {
        return heap_match_utils::does_memory_match_to_range(range_address, range_size, user_address(), user_requested_size());
    }

    uint64_t process_heap_entry::get_nt_heap_entry_check_block_start(heap_entry const& entry) const
    {
        if(user_address_ == 0)
        {
            return entry.address();
        }

        return user_address_;
    }

    uint64_t process_heap_entry::get_nt_heap_entry_check_block_size(heap_entry const& entry) const
    {
        if(user_address_ == 0)
        {
            return entry.size().count();
        }
        return user_size_.count();
    }

    heap_node_type process_heap_entry::get_heap_entry_node_type(heap_entry const& entry)
    {
        if(entry.is_lfh_entry())
        {
            return heap_node_type::nt_heap_lfh_entry;
        }

        if(entry.is_virtual_alloc())
        {
            return heap_node_type::nt_heap_virtual_entry;
        }

        return heap_node_type::nt_heap_segment_entry;
    }

    heap_node_type process_heap_entry::get_dph_entry_type(dph_entry const& entry)
    {
        return entry.is_virtual_allocation_entry() ? heap_node_type::dph_virtual_entry : heap_node_type::dph_entry;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void process_heap_entry::validate_entry() const
    {
    }
}
