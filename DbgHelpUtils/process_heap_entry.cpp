#include "process_heap_entry.h"

#include "dph_entry.h"
#include "heap_entry.h"
#include "heap_lfh_entry.h"
#include "heap_vs_entry.h"
#include "large_alloc_entry.h"
#include "page_range_descriptor.h"
#include "process_environment_block.h"

namespace dlg_help_utils::heap
{
    process_heap_entry::process_heap_entry(dph_entry const& entry)
    : peb_{entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    {
    }

    process_heap_entry::process_heap_entry(heap_entry const& entry)
    : peb_{entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    {
    }

    process_heap_entry::process_heap_entry(heap_lfh_entry const& entry)
    : peb_{entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    {
    }

    process_heap_entry::process_heap_entry(page_range_descriptor const& entry)
    : peb_{entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    {
    }

    process_heap_entry::process_heap_entry(heap_vs_entry const& entry)
    : peb_{entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    {
    }

    process_heap_entry::process_heap_entry(large_alloc_entry const& entry)
    : peb_{entry.peb()}
    , user_address_{entry.user_address()}
    , user_size_{entry.user_requested_size()}
    , allocation_stack_trace_{entry.allocation_stack_trace()}
    {
    }

    mini_dump_memory_stream process_heap_entry::user_data() const
    {
        uint64_t const size = user_requested_size().count();
        return peb_.walker().get_process_memory_stream(user_address(), size);
    }
}
