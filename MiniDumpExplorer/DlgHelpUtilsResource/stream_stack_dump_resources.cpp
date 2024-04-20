#include "pch.h"
#include "DbgHelpUtils/stream_stack_dump.h"

#include "Utility/GetResourceValue.h"

namespace dlg_help_utils::stream_stack_dump::resources
{
    std::wstring get_inline_string()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpInline");
    }

    std::wstring get_parameters_title()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpParametersTitle");
    }

    std::wstring get_local_variables_title()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpLocalVariablesTitle");
    }

    std::wstring get_symbol_name_prefix()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpSymbolNamePrefix");
    }

    std::wstring get_symbol_displacement_prefix()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpSymbolDisplacementPrefix");
    }

    std::wstring get_module_displacement_prefix()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpModuleDisplacementPrefix");
    }

    std::wstring get_unknown_variable_type()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpUnknownVariableType");
    }

    std::wstring get_file_name_line_number_separator()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpFileNameLineNumberSeparator");
    }

    std::wstring get_failed_to_find_address_prefix()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpFailedToFindAddressPrefix");
    }

    std::wstring get_failed_to_find_address_name_address_separator()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpFailedToFindAddressNameAddressSeparator");
    }

    std::wstring get_failed_to_find_address_postfix()
    {
        return Utility::GetResourceValue(L"Resources/StreamStackDumpFailedToFindAddressPostfix");
    }
}
