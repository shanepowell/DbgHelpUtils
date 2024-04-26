#include "time_units.h"

#include <sstream>

#include "chrono_unit_convert_to_string.h"
#include "get_label_type_from_labels.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::time_units
{
    namespace
    {
        std::wstring print_value(time_unit_type const type, auto const us1, auto const us2)
        {
            auto const& str = get_label_strings(type);
            std::wstringstream ss;
            chrono_unit_utilities::convert_to_string(ss, us1, type == time_unit_type::filetime_nanosecond ? L"00"s : L""s, us2, std::get<0>(str), std::get<1>(str));
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

            if (tb.count() != 0)
            {
                return print_value(map_to_time_type<Tsz>::type, tb, std::chrono::duration_cast<Ysz>(ms));
            }

            return to_string_internal<TBsz, Ysz, Args...>(ms);
        }
    } // namespace


    std::wstring to_timespan_wstring(filetime_nanoseconds fns)
    {
        const auto days_value = std::chrono::duration_cast<days>(fns);
        fns -= days_value;
        const auto hours_value = std::chrono::duration_cast<hours>(fns);
        fns -= hours_value;
        const auto minutes_value = std::chrono::duration_cast<minutes>(fns);
        fns -= minutes_value;
        const auto seconds_value = std::chrono::duration_cast<std::chrono::seconds>(fns);
        fns -= seconds_value;
        const auto milliseconds_value = std::chrono::duration_cast<std::chrono::milliseconds>(fns);
        fns -= milliseconds_value;
        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(fns);

        auto fmt = locale_formatting::get_default_number_format_w();

        if(days_value.count() != 0)
        {
            if(ns.count() > 0)
            {
                return std::format(L"{0}{6}{1:0>2}:{2:0>2}:{3:0>2}{6}{4:0>3}{6}{5:0>6}", locale_formatting::to_wstring(days_value.count()), std::abs(hours_value.count()), std::abs(minutes_value.count()), std::abs(seconds_value.count()), std::abs(milliseconds_value.count()), std::abs(ns.count()), fmt.lpDecimalSep);
            }

            if(milliseconds_value.count() > 0)
            {
                return std::format(L"{0}{5}{1:0>2}:{2:0>2}:{3:0>2}{5}{4:0>3}", locale_formatting::to_wstring(days_value.count()), std::abs(hours_value.count()), std::abs(minutes_value.count()), std::abs(seconds_value.count()), std::abs(milliseconds_value.count()), fmt.lpDecimalSep);
            }

            return std::format(L"{0}{4}{1:0>2}:{2:0>2}:{3:0>2}", locale_formatting::to_wstring(days_value.count()), std::abs(hours_value.count()), std::abs(minutes_value.count()), std::abs(seconds_value.count()), fmt.lpDecimalSep);
        }

        if(ns.count() > 0)
        {
            return std::format(L"{0:0>2}:{1:0>2}:{2:0>2}{5}{3:0>3}{5}{4:0>6}", hours_value.count(), std::abs(minutes_value.count()), std::abs(seconds_value.count()), std::abs(milliseconds_value.count()), std::abs(ns.count()), fmt.lpDecimalSep);
        }

        if(milliseconds_value.count() > 0)
        {
            return std::format(L"{0:0>2}:{1:0>2}:{2:0>2}{4}{3:0>3}", hours_value.count(), std::abs(minutes_value.count()), std::abs(seconds_value.count()), std::abs(milliseconds_value.count()), fmt.lpDecimalSep);
        }

        return std::format(L"{0:0>2}:{1:0>2}:{2:0>2}", hours_value.count(), std::abs(minutes_value.count()), std::abs(seconds_value.count()));
    }

    std::wstring to_wstring(filetime_nanoseconds const ns)
    {
        return to_string_internal<filetime_nanoseconds, years, months, weeks, days, hours, minutes, std::chrono::seconds, std::chrono::milliseconds, filetime_nanoseconds>(ns);
    }

    std::wstring to_nanoseconds_wstring(filetime_nanoseconds const ns)
    {
        return print_value(time_unit_type::filetime_nanosecond, ns, filetime_nanoseconds{0});
    }

    std::wstring to_milliseconds_wstring(filetime_nanoseconds ns)
    {
        const auto value = std::chrono::duration_cast<std::chrono::milliseconds>(ns);
        ns -= value;
        return print_value(time_unit_type::millisecond, value, ns);
    }

    std::wstring to_seconds_wstring(filetime_nanoseconds ns)
    {
        const auto value = std::chrono::duration_cast<std::chrono::seconds>(ns);
        ns -= value;
        return print_value(time_unit_type::second, value, ns);
    }

    std::wstring to_minutes_wstring(filetime_nanoseconds ns)
    {
        const auto value = std::chrono::duration_cast<minutes>(ns);
        ns -= value;
        return print_value(time_unit_type::minute, value, std::chrono::duration_cast<std::chrono::seconds>(ns));
    }

    std::wstring to_hours_wstring(filetime_nanoseconds ns)
    {
        const auto value = std::chrono::duration_cast<hours>(ns);
        ns -= value;
        return print_value(time_unit_type::hour, value, std::chrono::duration_cast<minutes>(ns));
    }

    std::wstring to_days_wstring(filetime_nanoseconds ns)
    {
        const auto value = std::chrono::duration_cast<days>(ns);
        ns -= value;
        return print_value(time_unit_type::day, value, std::chrono::duration_cast<hours>(ns));
    }

    std::wstring to_weeks_wstring(filetime_nanoseconds ns)
    {
        const auto value = std::chrono::duration_cast<std::chrono::weeks>(ns);
        ns -= value;
        return print_value(time_unit_type::week, value, std::chrono::duration_cast<std::chrono::days>(ns));
    }

    std::wstring to_months_wstring(filetime_nanoseconds ns)
    {
        const auto value = std::chrono::duration_cast<months>(ns);
        ns -= value;
        return print_value(time_unit_type::month, value, std::chrono::duration_cast<weeks>(ns));
    }

    std::wstring to_years_wstring(filetime_nanoseconds ns)
    {
        const auto value = std::chrono::duration_cast<years>(ns);
        ns -= value;
        return print_value(time_unit_type::year, value, std::chrono::duration_cast<months>(ns));
    }

    std::wstring const& get_label_string(const time_unit_type time_type, const string_type type)
    {
        auto const& str = get_label_strings(time_type);
        return type == string_type::singular ? std::get<0>(str) : std::get<1>(str);
    }

    std::tuple<std::wstring, std::wstring> const& get_label_strings(const time_unit_type type)
    {
        auto const& type_strings = resources::get_type_strings();
        return type_strings.at(type);
    }

    time_unit_type get_label_type(std::wstring_view const label)
    {
        auto const& type_strings = resources::get_type_strings();
        if(auto const type = utilities::get_label_type_from_labels(label, type_strings); type.has_value())
        {
            return type.value();
        }

        throw exceptions::wide_runtime_error{std::format(L"Unknown time units type [{}]", label)};
    }
}
