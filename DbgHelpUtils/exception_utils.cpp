#include "exception_utils.h"

#include "windows_setup.h"
#include <sstream>

using namespace std::string_literals;

namespace dlg_help_utils::exception_utils
{
    std::wstring_view exception_code_to_string(uint32_t const exception_code)
    {
        auto const& exception_code_descriptions = resources::get_exception_code_descriptions();;
        if (auto const it = exception_code_descriptions.find(exception_code); it != exception_code_descriptions.end())
        {
            return it->second;
        }

        return resources::get_unknown_exception_string();
    }

    std::wstring exception_flags_to_string(uint32_t const exception_flags)
    {
        std::wostringstream ss;

        ss << resources::get_exception_continuable(exception_flags & EXCEPTION_NONCONTINUABLE);
        ss << resources::get_exception_unwinding(exception_flags & EXCEPTION_UNWINDING);
        ss << resources::get_exception_exit_unwind(exception_flags & EXCEPTION_EXIT_UNWIND);
        ss << resources::get_exception_stack_invalid(exception_flags & EXCEPTION_STACK_INVALID);
        ss << resources::get_exception_nesting_level(exception_flags & EXCEPTION_NESTED_CALL);
        ss << resources::get_exception_target_unwind(exception_flags & EXCEPTION_TARGET_UNWIND);
        ss << resources::get_exception_collided_unwind(exception_flags & EXCEPTION_COLLIDED_UNWIND);

        return std::move(ss).str();
    }
}
