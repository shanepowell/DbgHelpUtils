#include "lfh_subsegment_location_utils.h"

#include "flags_string_utils.h"

using namespace std::string_literals;

namespace
{
    using dlg_help_utils::lfh_subsegment_location_utils::location;

    std::unordered_map<location, std::wstring> g_location_descriptions =
    {
        {location::available_subsegment_list,   L"AvailableSubsegmentList"s},
        {location::full_subsegment_list,        L"FullSubsegmentList"s},
        {location::retiring_subsegment_list ,   L"RetiringSubsegmentList"s},
    };
}

namespace dlg_help_utils::lfh_subsegment_location_utils
{
    std::wstring dump_location_to_string(location const value)
    {
        return flags_string_utils::generate_enum_string(value, g_location_descriptions);
    }
}
