#pragma once
#include <string>
#include <vector>

namespace dlg_help_utils::memory_info_utils
{
    // see https://docs.microsoft.com/en-us/windows/win32/memory/memory-protection-constants
    std::wstring memory_protection_options_to_string(uint32_t memory_protection_options);
    std::vector<std::wstring_view> memory_protection_options_to_strings(uint32_t memory_protection_options);

    // see https://docs.microsoft.com/en-us/windows/win32/api/minidumpapiset/ns-minidumpapiset-minidump_memory_info
    std::wstring memory_state_to_string(uint32_t state);
    std::wstring memory_type_to_string(uint32_t type);

    std::wstring system_memory_info_flags_to_string(uint16_t flags);
    std::vector<std::wstring_view> system_memory_info_flags_to_strings(uint16_t flags);
}
