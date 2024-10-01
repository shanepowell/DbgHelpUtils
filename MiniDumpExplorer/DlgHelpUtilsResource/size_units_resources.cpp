#include "pch.h"
#include "DbgHelpUtils/size_units.h"

#include <format>
#include <unordered_map>

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::size_units::resources
{
    namespace
    {
        std::unordered_map<size_unit_type, std::wstring> const enum_names
        {
            { size_unit_type::exabytes, L"Exabytes"s },
            { size_unit_type::petabytes, L"Petabytes"s },
            { size_unit_type::terabytes, L"Terabytes"s },
            { size_unit_type::gigabytes, L"Gigabytes"s },
            { size_unit_type::megabytes, L"Megabytes"s },
            { size_unit_type::kilobytes, L"Kilobytes"s },
            { size_unit_type::bytes, L"Bytes"s }
        };

        std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> populate_type_strings()
        {
            std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> rv;
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            for(auto const& [type, name] : enum_names)
            {
                rv[type] = std::make_tuple(
                    std::wstring{rm.MainResourceMap().GetValue(std::format(L"Resources/{}Singular", name)).ValueAsString()},
                    std::wstring{rm.MainResourceMap().GetValue(std::format(L"Resources/{}Plural", name)).ValueAsString()},
                    std::wstring{rm.MainResourceMap().GetValue(std::format(L"Resources/{}Compact", name)).ValueAsString()}
                );
            }

            return rv;
        }
    }

    std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> const& get_type_strings()  // NOLINT(misc-use-internal-linkage)
    {
        static std::unordered_map const data{populate_type_strings()};
        return data;
    }
}