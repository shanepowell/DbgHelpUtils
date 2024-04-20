#include "pch.h"
#include "DbgHelpUtils/time_units.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

using namespace std::string_literals;

namespace dlg_help_utils::time_units::resources
{
    namespace
    {
        std::unordered_map<time_unit_type, std::wstring> const enum_names
        {
            { time_unit_type::year, L"Year"s },
            { time_unit_type::month, L"Month"s },
            { time_unit_type::week, L"Week"s },
            { time_unit_type::day, L"Day"s },
            { time_unit_type::hour, L"Hour"s },
            { time_unit_type::minute, L"Minute"s },
            { time_unit_type::second, L"Second"s },
            { time_unit_type::millisecond, L"Millisecond"s }
        };

        std::unordered_map<time_unit_type, std::tuple<std::wstring, std::wstring>> populate_type_strings()
        {
            std::unordered_map<time_unit_type, std::tuple<std::wstring, std::wstring>> rv;
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            for(auto const& [type, name] : enum_names)
            {
                rv[type] = std::make_tuple(
                    std::wstring{rm.MainResourceMap().GetValue(std::format(L"Resources/{}Singular", name)).ValueAsString()},
                    std::wstring{rm.MainResourceMap().GetValue(std::format(L"Resources/{}Plural", name)).ValueAsString()}
                );
            }

            return rv;
        }
    }

    std::unordered_map<time_unit_type, std::tuple<std::wstring, std::wstring>> const& get_type_strings()
    {
        static std::unordered_map const data{populate_type_strings()};
        return data;
    }
}
