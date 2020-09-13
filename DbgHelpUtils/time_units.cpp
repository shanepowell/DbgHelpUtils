#include "time_units.h"

#include <sstream>

#include "chrono_unit_convert_to_string.h"

namespace dlg_help_utils::time_units
{
    namespace
    {
        std::unordered_map<time_unit_type, std::pair<std::wstring, std::wstring>> populate_default_strings()
        {
            std::unordered_map<time_unit_type, std::pair<std::wstring, std::wstring>> rv;
            rv[time_unit_type::year] = make_pair(std::wstring{L"year"}, std::wstring{L"years"});
            rv[time_unit_type::month] = make_pair(std::wstring{L"month"}, std::wstring{L"months"});
            rv[time_unit_type::week] = make_pair(std::wstring{L"week"}, std::wstring{L"weeks"});
            rv[time_unit_type::day] = make_pair(std::wstring{L"day"}, std::wstring{L"days"});
            rv[time_unit_type::hour] = make_pair(std::wstring{L"hour"}, std::wstring{L"hours"});
            rv[time_unit_type::minute] = make_pair(std::wstring{L"minute"}, std::wstring{L"minutes"});
            rv[time_unit_type::second] = make_pair(std::wstring{L"second"}, std::wstring{L"seconds"});
            rv[time_unit_type::millisecond] = make_pair(std::wstring{L"millisecond"}, std::wstring{L"milliseconds"});
            return rv;
        }

        std::unordered_map<time_unit_type, std::pair<std::wstring, std::wstring>> const g_default_string_data =
            populate_default_strings();
        std::unordered_map<time_unit_type, std::pair<std::wstring, std::wstring>> g_user_string_data;
    } // namespace


    std::wstring to_wstring(std::chrono::milliseconds ms)
    {
        auto years_months_ms = ms;
        const auto yrs = std::chrono::duration_cast<years>(years_months_ms);
        years_months_ms -= yrs;
        const auto mons = std::chrono::duration_cast<months>(years_months_ms);
        years_months_ms -= mons;

        std::wstringstream ss;
        if ((yrs.count() > 0 || mons.count() > 0) && years_months_ms.count() == 0)
        {
            if (yrs.count() > 0)
            {
                auto const& str = get_label_strings(time_unit_type::year);
                chrono_unit_utilities::convert_to_string(ss, yrs, mons, str.first.c_str(), str.second.c_str());
            }
            else
            {
                ss << mons.count() << L" " << get_label_string(time_unit_type::month,
                                                               mons.count() == 1
                                                                   ? string_type::singular
                                                                   : string_type::plural);
            }
        }
        else
        {
            const auto wks = std::chrono::duration_cast<weeks>(ms);
            ms -= wks;
            const auto dys = std::chrono::duration_cast<days>(ms);
            ms -= dys;
            const auto hrs = std::chrono::duration_cast<std::chrono::hours>(ms);
            ms -= hrs;
            const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(ms);
            ms -= minutes;
            const auto secs = std::chrono::duration_cast<std::chrono::seconds>(ms);
            ms -= secs;

            if (wks.count() > 0)
            {
                auto const& str = get_label_strings(time_unit_type::week);
                chrono_unit_utilities::convert_to_string(ss, wks, dys, str.first.c_str(), str.second.c_str());
            }
            else if (dys.count() > 0)
            {
                auto const& str = get_label_strings(time_unit_type::day);
                chrono_unit_utilities::convert_to_string(ss, dys, hrs, str.first.c_str(), str.second.c_str());
            }
            else if (hrs.count() > 0)
            {
                auto const& str = get_label_strings(time_unit_type::hour);
                chrono_unit_utilities::convert_to_string(ss, hrs, minutes, str.first.c_str(), str.second.c_str());
            }
            else if (minutes.count() > 0)
            {
                auto const& str = get_label_strings(time_unit_type::minute);
                chrono_unit_utilities::convert_to_string(ss, minutes, secs, str.first.c_str(), str.second.c_str());
            }
            else if (secs.count() > 0)
            {
                auto const& str = get_label_strings(time_unit_type::second);
                chrono_unit_utilities::convert_to_string(ss, secs, ms, str.first.c_str(), str.second.c_str());
            }
            else
            {
                ss << ms.count() << L" " << get_label_string(time_unit_type::millisecond,
                                                             ms.count() == 1
                                                                 ? string_type::singular
                                                                 : string_type::plural);
            }
        }

        return ss.str();
    }

    std::wstring const& get_label_string(const time_unit_type time_type, const string_type type)
    {
        auto const& str = get_label_strings(time_type);
        return type == string_type::singular ? str.first : str.second;
    }

    std::pair<std::wstring, std::wstring> const& get_label_strings(const time_unit_type type)
    {
        const auto it = g_user_string_data.find(type);
        if (it == g_user_string_data.end())
        {
            return g_default_string_data.at(type);
        }

        return it->second;
    }

    void set_label_strings(std::unordered_map<time_unit_type, std::pair<std::wstring, std::wstring>> user_string_data)
    {
        g_user_string_data = std::move(user_string_data);
    }
}
