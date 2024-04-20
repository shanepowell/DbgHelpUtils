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

            if (tb.count() != 0)
            {
                return print_value(map_to_time_type<Tsz>::type, tb, std::chrono::duration_cast<Ysz>(ms));
            }

            return to_string_internal<TBsz, Ysz, Args...>(ms);
        }
    } // namespace


    std::wstring to_timespan_wstring(std::chrono::milliseconds ms)
    {
        const auto days_value = std::chrono::duration_cast<days>(ms);
        ms -= days_value;
        const auto hours_value = std::chrono::duration_cast<hours>(ms);
        ms -= hours_value;
        const auto minutes_value = std::chrono::duration_cast<minutes>(ms);
        ms -= minutes_value;
        const auto seconds_value = std::chrono::duration_cast<std::chrono::seconds>(ms);
        ms -= seconds_value;
        auto fmt = locale_formatting::get_default_number_format_w();

        if(days_value.count() != 0)
        {
            if(ms.count() > 0)
            {
                return std::format(L"{0}{5}{1:0>2}:{2:0>2}:{3:0>2}{5}{4:0>3}", locale_formatting::to_wstring(days_value.count()), std::abs(hours_value.count()), std::abs(minutes_value.count()), std::abs(seconds_value.count()), std::abs(ms.count()), fmt.lpDecimalSep);
            }

            return std::format(L"{0}{4}{1:0>2}:{2:0>2}:{3:0>2}", locale_formatting::to_wstring(days_value.count()), std::abs(hours_value.count()), std::abs(minutes_value.count()), std::abs(seconds_value.count()), fmt.lpDecimalSep);
        }

        if(ms.count() > 0)
        {
            return std::format(L"{0:0>2}:{1:0>2}:{2:0>2}{4}{3:0>3}", hours_value.count(), std::abs(minutes_value.count()), std::abs(seconds_value.count()), std::abs(ms.count()), fmt.lpDecimalSep);
        }

        return std::format(L"{0:0>2}:{1:0>2}:{2:0>2}", hours_value.count(), std::abs(minutes_value.count()), std::abs(seconds_value.count()));
    }

    std::wstring to_wstring(std::chrono::milliseconds const ms)
    {
        return to_string_internal<std::chrono::milliseconds, years, months, weeks, days, hours, minutes, std::chrono::seconds, std::chrono::milliseconds>(ms);
    }

    std::wstring to_milliseconds_wstring(std::chrono::milliseconds const ms)
    {
        return print_value(time_unit_type::millisecond, ms, std::chrono::milliseconds{0});
    }

    std::wstring to_seconds_wstring(std::chrono::milliseconds ms)
    {
        const auto value = std::chrono::duration_cast<std::chrono::seconds>(ms);
        ms -= value;
        return print_value(time_unit_type::second, value, ms);
    }

    std::wstring to_minutes_wstring(std::chrono::milliseconds ms)
    {
        const auto value = std::chrono::duration_cast<minutes>(ms);
        ms -= value;
        return print_value(time_unit_type::minute, value, std::chrono::duration_cast<std::chrono::seconds>(ms));
    }

    std::wstring to_hours_wstring(std::chrono::milliseconds ms)
    {
        const auto value = std::chrono::duration_cast<hours>(ms);
        ms -= value;
        return print_value(time_unit_type::hour, value, std::chrono::duration_cast<minutes>(ms));
    }

    std::wstring to_days_wstring(std::chrono::milliseconds ms)
    {
        const auto value = std::chrono::duration_cast<days>(ms);
        ms -= value;
        return print_value(time_unit_type::day, value, std::chrono::duration_cast<hours>(ms));
    }

    std::wstring to_weeks_wstring(std::chrono::milliseconds ms)
    {
        const auto value = std::chrono::duration_cast<std::chrono::weeks>(ms);
        ms -= value;
        return print_value(time_unit_type::week, value, std::chrono::duration_cast<std::chrono::days>(ms));
    }

    std::wstring to_months_wstring(std::chrono::milliseconds ms)
    {
        const auto value = std::chrono::duration_cast<months>(ms);
        ms -= value;
        return print_value(time_unit_type::month, value, std::chrono::duration_cast<weeks>(ms));
    }

    std::wstring to_years_wstring(std::chrono::milliseconds ms)
    {
        const auto value = std::chrono::duration_cast<years>(ms);
        ms -= value;
        return print_value(time_unit_type::year, value, std::chrono::duration_cast<months>(ms));
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
