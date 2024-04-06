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
    std::vector<std::wstring_view> exception_flags_to_list(uint32_t exception_flags);

    namespace resources
    {
        std::unordered_map<uint32_t, std::wstring> const& get_exception_code_descriptions();
        std::wstring_view get_unknown_exception_string();
        std::wstring_view get_exception_continuable(bool exception_continuable);
        std::wstring_view get_exception_unwinding(bool exception_unwinding);
        std::wstring_view get_exception_exit_unwind(bool exception_exit_unwind);
        std::wstring_view get_exception_stack_invalid(bool exception_stack_invalid);
        std::wstring_view get_exception_nesting_level(bool exception_nesting_level);
        std::wstring_view get_exception_target_unwind(bool exception_target_unwind);
        std::wstring_view get_exception_collided_unwind(bool exception_collided_unwind);
    }
}
