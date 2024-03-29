﻿#pragma once
#include <string>
#include <vector>

namespace dlg_help_utils::thread_info_utils
{
    std::wstring dump_flags_to_string(uint32_t dump_flags);
    std::vector<std::wstring_view> dump_flags_to_strings(uint32_t dump_flags);
}
