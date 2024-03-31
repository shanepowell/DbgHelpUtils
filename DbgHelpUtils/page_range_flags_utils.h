#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace dlg_help_utils::page_range_flags_utils
{
    enum class page_range_flags : uint16_t {
        PAGE_RANGE_FLAGS_LFH_SUBSEGMENT = 0x01,
        PAGE_RANGE_FLAGS_COMMITTED      = 0x02,
        PAGE_RANGE_FLAGS_ALLOCATED      = 0x04,
        PAGE_RANGE_FLAGS_FIRST          = 0x08,
        PAGE_RANGE_FLAGS_VS_SUBSEGMENT  = 0x20,

        PAGE_RANGE_FREE                 = PAGE_RANGE_FLAGS_COMMITTED,
        PAGE_RANGE_VARIABLE_ALLOC       = PAGE_RANGE_FLAGS_LFH_SUBSEGMENT | PAGE_RANGE_FLAGS_COMMITTED | PAGE_RANGE_FLAGS_ALLOCATED | PAGE_RANGE_FLAGS_FIRST,
        PAGE_RANGE_LFH_SUBSEGMENT       = PAGE_RANGE_FLAGS_LFH_SUBSEGMENT | PAGE_RANGE_FLAGS_COMMITTED | PAGE_RANGE_FLAGS_FIRST,
        PAGE_RANGE_BACKEND_SUBSEGMENT   = PAGE_RANGE_FLAGS_LFH_SUBSEGMENT | PAGE_RANGE_FLAGS_COMMITTED,
    };

    std::wstring dump_page_range_flags_to_string(page_range_flags flags);
    std::vector<std::wstring_view> dump_page_range_flags_to_strings(page_range_flags flags);

    std::wstring dump_page_range_to_string(page_range_flags value);
}