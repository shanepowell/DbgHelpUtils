#pragma once
#include <string_view>

namespace dlg_help_utils::lfh_subsegment_location_utils
{
    enum class location
    {
        available_subsegment_list = 0,
        full_subsegment_list = 1,
        retiring_subsegment_list = 2
    };

    std::wstring_view dump_page_range_flags_to_string(location value);
}
