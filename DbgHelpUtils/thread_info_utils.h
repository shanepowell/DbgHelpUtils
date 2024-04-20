#pragma once
#include <string>
#include <vector>

namespace dlg_help_utils::thread_info_utils
{
    std::wstring dump_flags_to_string(uint32_t dump_flags);
    std::vector<std::wstring> dump_flags_to_strings(uint32_t dump_flags);

    std::wstring thread_priority_class_to_string(uint32_t thread_priority_class);
}
