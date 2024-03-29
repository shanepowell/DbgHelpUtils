#include "time_units.h"

using namespace std::string_literals;

namespace dlg_help_utils::time_units::details
{
    namespace
    {
        std::unordered_map<time_unit_type, std::tuple<std::wstring, std::wstring>> populate_default_strings()
        {
            std::unordered_map<time_unit_type, std::tuple<std::wstring, std::wstring>> rv;
            rv[time_unit_type::year] = make_pair(L"year"s, L"years"s);
            rv[time_unit_type::month] = make_pair(L"month"s, L"months"s);
            rv[time_unit_type::week] = make_pair(L"week"s, L"weeks"s);
            rv[time_unit_type::day] = make_pair(L"day"s, L"days"s);
            rv[time_unit_type::hour] = make_pair(L"hour"s, L"hours"s);
            rv[time_unit_type::minute] = make_pair(L"minute"s, L"minutes"s);
            rv[time_unit_type::second] = make_pair(L"second"s, L"seconds"s);
            rv[time_unit_type::millisecond] = make_pair(L"millisecond"s, L"milliseconds"s);
            return rv;
        }

        std::unordered_map const g_default_string_data{populate_default_strings()};

    }

    std::unordered_map<time_unit_type, std::tuple<std::wstring, std::wstring>> const& get_type_strings()
    {
        return g_default_string_data;
    }
}
