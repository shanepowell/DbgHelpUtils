#include "exception_utils.h"

#include "windows_setup.h"
#include <sstream>

#include "flags_string_utils.h"

using namespace std::string_literals;

namespace dlg_help_utils::exception_utils
{
    std::wstring_view exception_code_to_string(uint32_t const exception_code)
    {
        auto const& exception_code_descriptions = resources::get_exception_code_descriptions();
        if (auto const it = exception_code_descriptions.find(exception_code); it != exception_code_descriptions.end())
        {
            return it->second;
        }

        return resources::get_unknown_exception_string();
    }
}
