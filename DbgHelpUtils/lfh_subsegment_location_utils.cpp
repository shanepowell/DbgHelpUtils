#include "lfh_subsegment_location_utils.h"

#include <unordered_map>

using namespace std::string_view_literals;

namespace
{

    using dlg_help_utils::lfh_subsegment_location_utils::location;

    std::unordered_map<uint32_t, std::wstring_view> g_location_descriptions =
    {
        {static_cast<uint32_t>(location::available_subsegment_list), L"AvailableSubsegmentList"sv},
        {static_cast<uint32_t>(location::full_subsegment_list     ), L"FullSubsegmentList"sv},
        {static_cast<uint32_t>(location::retiring_subsegment_list ), L"RetiringSubsegmentList"sv},
    };
}

namespace dlg_help_utils::lfh_subsegment_location_utils
{
    std::wstring_view dump_page_range_flags_to_string(location value)
    {
        if (auto const it = g_location_descriptions.find(static_cast<uint32_t>(value)); it != g_location_descriptions.end())
        {
            return it->second;
        }

        return L"unknown"sv;
    }
}
