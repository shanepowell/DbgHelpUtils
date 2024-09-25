#pragma once
#include <string>

namespace dlg_help_utils::lfh_subsegment_location_utils
{
    enum class location : uint8_t
    {
        available_subsegment_list = 0,
        full_subsegment_list = 1,
        retiring_subsegment_list = 2
    };

    std::wstring dump_location_to_string(location value);
}
