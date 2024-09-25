#pragma once

namespace dlg_help_utils::heap
{
    enum class heap_node_type : uint8_t
    {
        nt_heap_lfh_entry,
        nt_heap_segment_entry,
        nt_heap_virtual_entry,
        segment_backend_entry,
        segment_entry,
        segment_lfh_entry,
        segment_large_entry,
        dph_entry,
        dph_virtual_entry,
        memory_range
    };
}