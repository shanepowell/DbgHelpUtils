#include "time_units.h"

#include <sstream>

#include "chrono_unit_convert_to_string.h"
#include "string_compare.h"
#include "wide_runtime_error.h"

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

        std::wstring print_value(time_unit_type const type, auto const us1, auto const us2)
        {
            auto const& str = get_label_strings(type);
            std::wstringstream ss;
            chrono_unit_utilities::convert_to_string(ss, us1, us2, std::get<0>(str), std::get<1>(str));
            return std::move(ss).str();
        }

        template<typename TBsz, typename Tsz>
        std::wstring to_string_internal(TBsz ms)
        {
            return print_value(map_to_time_type<Tsz>::type, ms, TBsz{0});
        }

        template<typename TBsz, typename Tsz, typename Ysz, typename ...Args>
        std::wstring to_string_internal(TBsz ms)
        {
            const auto tb = std::chrono::duration_cast<Tsz>(ms);
            ms -= tb;

            if (tb.count() > 0)
            {
                return print_value(map_to_time_type<Tsz>::type, tb, std::chrono::duration_cast<Ysz>(ms));
            }

            return to_string_internal<TBsz, Ysz, Args...>(ms);
        }

        std::optional<time_unit_type> get_label_type_from_labels(std::wstring_view const& label, std::unordered_map<time_unit_type, std::pair<std::wstring, std::wstring>> const& labels)
        {
            for(auto const& [type, label_strings] : labels)
            {
                if(auto const& [singular, plural] = label_strings;
                    string_utils::iequals(singular, label) ||
                    string_utils::iequals(plural, label))
                {
                    return type;
                }
            }

            return std::nullopt;
        }
    } // namespace


    std::wstring to_wstring(std::chrono::milliseconds const ms)
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
                const auto& [name, plural_name] = get_label_strings(time_unit_type::year);
                chrono_unit_utilities::convert_to_string(ss, yrs, mons, name, plural_name);
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
            return to_string_internal<std::chrono::milliseconds, weeks, days, std::chrono::hours, std::chrono::minutes, std::chrono::seconds, std::chrono::milliseconds>(ms);
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

    time_unit_type get_label_type(std::wstring_view const label)
    {
        if(auto const type = get_label_type_from_labels(label, g_user_string_data); type.has_value())
        {
            return type.value();
        }
        if(auto const type = get_label_type_from_labels(label, g_default_string_data); type.has_value())
        {
            return type.value();
        }

        throw exceptions::wide_runtime_error{std::format(L"Unknown units type [{}]", label)};
    }
}
