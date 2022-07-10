#include "heap_lfh_entry.h"

#include "heap_lfh_context.h"
#include "process_environment_block.h"
#include "segment_heap_utils.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils::heap
{
    heap_lfh_entry::heap_lfh_entry(heap_lfh_context const& heap, uint64_t const heap_lfh_entry_address, size_units::base_16::bytes const block_size, allocated_t const allocated, has_unused_bytes_t const has_unused_bytes)
    : heap_{&heap}
    , heap_lfh_entry_address_{heap_lfh_entry_address}
    , block_size_{block_size}
    , allocated_{allocated}
    , has_unused_bytes_{has_unused_bytes}
    , unused_bytes_{get_unused_bytes()}
    , ust_address_{get_ust_address()}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& heap_lfh_entry::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_lfh_entry::peb() const
    {
        return heap().peb();
    }

    uint64_t heap_lfh_entry::user_address() const
    {
        return heap_lfh_entry_address();
    }

    size_units::base_16::bytes heap_lfh_entry::user_requested_size() const
    {
        return size_units::base_16::bytes{block_size().count() - unused_bytes().count()};
    }

    size_units::base_16::bytes heap_lfh_entry::get_unused_bytes() const
    {
        if(!has_unused_bytes() || !allocated())
        {
            return size_units::base_16::bytes{0};
        }

        return size_units::base_16::bytes{segment_heap_utils::read_extra_data(peb(), heap().stack_trace(), block_address(), block_size().count()).unused_bytes};
    }

    uint64_t heap_lfh_entry::get_ust_address() const
    {
        if(!peb().user_stack_db_enabled() || !allocated() || !has_unused_bytes())
        {
            return 0;
        }

        return segment_heap_utils::read_extra_data(peb(), heap().stack_trace(), block_address(), block_size().count()).ust_address;
    }

    std::vector<uint64_t> heap_lfh_entry::get_allocation_stack_trace() const
    {
        return heap().stack_trace().read_allocation_stack_trace(peb(), ust_address());
    }
}
