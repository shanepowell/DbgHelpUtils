#include "page_range_flags_utils.h"

#include <map>
#include <unordered_map>

#include "flags_string_utils.h"

using namespace std::string_view_literals;

namespace
{

    using dlg_help_utils::page_range_flags_utils::page_range_flags;

   std::map<uint32_t, std::wstring_view> const dump_page_range_flags =
    {
        {static_cast<uint32_t>(page_range_flags::PAGE_RANGE_FLAGS_LFH_SUBSEGMENT), L"LFH_SUBSEGMENT"sv},
        {static_cast<uint32_t>(page_range_flags::PAGE_RANGE_FLAGS_COMMITTED     ), L"COMMITTED"sv},
        {static_cast<uint32_t>(page_range_flags::PAGE_RANGE_FLAGS_ALLOCATED     ), L"ALLOCATED"sv},
        {static_cast<uint32_t>(page_range_flags::PAGE_RANGE_FLAGS_FIRST         ), L"FIRST"sv},
        {static_cast<uint32_t>(page_range_flags::PAGE_RANGE_FLAGS_VS_SUBSEGMENT ), L"VS_SUBSEGMENT"sv},
    };

    std::unordered_map<uint32_t, std::wstring_view> g_page_range_descriptions =
    {
        {static_cast<uint32_t>(page_range_flags::PAGE_RANGE_FREE              ), L"Free Page Range"sv},
        {static_cast<uint32_t>(page_range_flags::PAGE_RANGE_VARIABLE_ALLOC    ), L"Variable Alloc"sv},
        {static_cast<uint32_t>(page_range_flags::PAGE_RANGE_LFH_SUBSEGMENT    ), L"LFH Subsegment"sv},
        {static_cast<uint32_t>(page_range_flags::PAGE_RANGE_BACKEND_SUBSEGMENT), L"Backend Allocation"sv},
    };
}

namespace dlg_help_utils::page_range_flags_utils
{
    std::wstring dump_page_range_flags_to_string(page_range_flags const flags)
    {
        return flags_string_utils::generate_flags_string(static_cast<uint32_t>(flags), dump_page_range_flags);
    }

    std::vector<std::wstring_view> dump_page_range_flags_to_strings(page_range_flags const flags)
    {
        return flags_string_utils::generate_flags_strings(static_cast<uint32_t>(flags), dump_page_range_flags);
    }

    std::wstring_view dump_page_range_to_string(page_range_flags const value)
    {
        if (auto const it = g_page_range_descriptions.find(static_cast<uint32_t>(value)); it != g_page_range_descriptions.end())
        {
            return it->second;
        }

        return L"unknown"sv;
    }
}
