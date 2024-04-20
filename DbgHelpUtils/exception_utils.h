#pragma once
#include <string>
#include <unordered_map>

// missing from windows headers, so EXCEPTION_POSSIBLE_DEADLOCK failed unless we define it to be found
enum : uint64_t
{
    STATUS_POSSIBLE_DEADLOCK = 0xC0000194L
};

namespace dlg_help_utils::exception_utils
{
    std::wstring_view exception_code_to_string(uint32_t exception_code);
    std::vector<std::wstring> exception_flags_to_list(uint32_t exception_flags);

    namespace resources
    {
        std::unordered_map<uint32_t, std::wstring> const& get_exception_code_descriptions();
        std::wstring_view get_unknown_exception_string();
    }
}
