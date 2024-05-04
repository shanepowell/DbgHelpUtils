#include "pch.h"
#include "DbgHelpUtils/function_info_utils.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/flags_string_utils.h"

using namespace std::string_literals;

namespace 
{
    std::unordered_map<dlg_help_utils::function_table_entry_type, std::wstring> const& function_table_entry_types()
    {
        using dlg_help_utils::function_table_entry_type;

        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<function_table_entry_type, std::wstring> function_table_entry_types =
        {
            {function_table_entry_type::unknown,                    std::wstring{rm.MainResourceMap().GetValue(L"Resources/function_table_entry_type_unknown").ValueAsString()}},
            {function_table_entry_type::fpo_data,                   std::wstring{rm.MainResourceMap().GetValue(L"Resources/function_table_entry_type_fpo_data").ValueAsString()}},
            {function_table_entry_type::image_function_entry,       std::wstring{rm.MainResourceMap().GetValue(L"Resources/function_table_entry_type_mage_function_entry").ValueAsString()}},
            {function_table_entry_type::image_function_entry_64,    std::wstring{rm.MainResourceMap().GetValue(L"Resources/function_table_entry_type_image_function_entry_64").ValueAsString()}},
        };

        return function_table_entry_types;
    }
}

namespace dlg_help_utils::function_info_utils
{
    std::wstring function_entry_type_to_string(function_table_entry_type const type)
    {
        return flags_string_utils::generate_enum_string(type, function_table_entry_types());
    }
}
