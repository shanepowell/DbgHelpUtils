#include "stream_stack_dump.h"

using namespace std::string_literals;

namespace dlg_help_utils::stream_stack_dump::resources
{
    std::wstring get_inline_string()
    {
        return L"Inline"s;
    }

    std::wstring get_parameters_title()
    {
        return L"Parameters"s;
    }

    std::wstring get_local_variables_title()
    {
        return L"Local Variables"s;
    }

    std::wstring get_symbol_name_prefix()
    {
        return L"!"s;
    }

    std::wstring get_symbol_displacement_prefix()
    {
        return L"+"s;
    }

    std::wstring get_module_displacement_prefix()
    {
        return L"+"s;
    }

    std::wstring get_unknown_variable_type()
    {
        return L"<unknown>"s;
    }

    std::wstring get_file_name_line_number_separator()
    {
        return L"@"s;
    }

    std::wstring get_failed_to_find_address_prefix()
    {
        return L"Failed to find ";
    }

    std::wstring get_failed_to_find_address_name_address_separator()
    {
        return L" address ";
    }

    std::wstring get_failed_to_find_address_postfix()
    {
        return L" in dump file";
    }
}
