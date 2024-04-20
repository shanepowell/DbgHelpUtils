#include "page_range_flags_utils.h"

#include "flags_string_utils.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{

    using dlg_help_utils::page_range_flags_utils::page_range_flags;

   std::unordered_map<page_range_flags, std::wstring> const dump_page_range_flags =
    {
        {page_range_flags::PAGE_RANGE_FLAGS_LFH_SUBSEGMENT, L"LFH_SUBSEGMENT"s},
        {page_range_flags::PAGE_RANGE_FLAGS_COMMITTED,      L"COMMITTED"s},
        {page_range_flags::PAGE_RANGE_FLAGS_ALLOCATED,      L"ALLOCATED"s},
        {page_range_flags::PAGE_RANGE_FLAGS_FIRST,          L"FIRST"s},
        {page_range_flags::PAGE_RANGE_FLAGS_VS_SUBSEGMENT,  L"VS_SUBSEGMENT"s},
    };

    std::unordered_map<page_range_flags, std::wstring> g_page_range_descriptions =
    {
        {page_range_flags::PAGE_RANGE_FREE,                 L"Free Page Range"s},
        {page_range_flags::PAGE_RANGE_VARIABLE_ALLOC,       L"Variable Alloc"s},
        {page_range_flags::PAGE_RANGE_LFH_SUBSEGMENT,       L"LFH Subsegment"s},
        {page_range_flags::PAGE_RANGE_BACKEND_SUBSEGMENT,   L"Backend Allocation"s},
    };
}

namespace dlg_help_utils::page_range_flags_utils
{
    std::wstring dump_page_range_flags_to_string(page_range_flags const flags)
    {
        return flags_string_utils::generate_flags_string(flags, dump_page_range_flags);
    }

    std::vector<std::wstring> dump_page_range_flags_to_strings(page_range_flags const flags)
    {
        return flags_string_utils::generate_flags_strings(flags, dump_page_range_flags);
    }

    std::wstring dump_page_range_to_string(page_range_flags const value)
    {
        return flags_string_utils::generate_enum_string(value, g_page_range_descriptions);
    }
}
