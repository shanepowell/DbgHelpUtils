#pragma once

// ReSharper disable once CommentTypo
// Found in "$(VSINSTALLDIR)\DIA SDK\include"
#include <cvconst.h>

#include "symbol_type_info.h"

namespace dlg_help_utils::dbg_help
{
    struct registry_info
    {
        CV_HREG_e register_type{};
        uint64_t value{};
        uint64_t value_size{};
    };

    struct frame_data_info
    {
        int32_t data_offset{};
        uint64_t data_address{};
        uint32_t data_size{};
    };

    struct variable
    {
        symbol_type_info symbol_info;
        std::optional<registry_info> registry_value{};
        std::optional<frame_data_info> frame_data{};
    };
}
