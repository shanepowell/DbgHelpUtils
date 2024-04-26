#pragma once

#include <chrono>
#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include <ostream>
#include <unordered_map>

namespace dlg_help_utils::time_units
{
    using filetime_nanoseconds   = std::chrono::duration<long long, std::ratio<1, 10000000>>;

    using minutes      = std::chrono::duration<long long, std::ratio<60>>;
    using hours        = std::chrono::duration<long long, std::ratio<3600>>;
    using days         = std::chrono::duration<long long, std::ratio_multiply<std::ratio<24>, hours::period>>;
    using weeks        = std::chrono::duration<long long, std::ratio_multiply<std::ratio<7>, days::period>>;
    using years        = std::chrono::duration<long long, std::ratio_multiply<std::ratio<146097, 400>, days::period>>;
    using months       = std::chrono::duration<long long, std::ratio_divide<years::period, std::ratio<12>>>;

    std::wstring to_timespan_wstring(filetime_nanoseconds fns);
    std::wstring to_wstring(filetime_nanoseconds ns);
    std::wstring to_nanoseconds_wstring(filetime_nanoseconds ns);
    std::wstring to_milliseconds_wstring(filetime_nanoseconds ns);
    std::wstring to_seconds_wstring(filetime_nanoseconds ns);
    std::wstring to_minutes_wstring(filetime_nanoseconds ns);
    std::wstring to_hours_wstring(filetime_nanoseconds ns);
    std::wstring to_days_wstring(filetime_nanoseconds ns);
    std::wstring to_weeks_wstring(filetime_nanoseconds ns);
    std::wstring to_months_wstring(filetime_nanoseconds ns);
    std::wstring to_years_wstring(filetime_nanoseconds ns);

    enum class time_unit_type
    {
        year,
        month,
        week,
        day,
        hour,
        minute,
        second,
        millisecond,
        filetime_nanosecond
    };

    template<typename T> struct map_to_time_type {};
    template<> struct map_to_time_type<years> { static constexpr time_unit_type type = time_unit_type::year; };
    template<> struct map_to_time_type<months> { static constexpr time_unit_type type = time_unit_type::month; };
    template<> struct map_to_time_type<weeks> { static constexpr time_unit_type type = time_unit_type::week; };
    template<> struct map_to_time_type<days> { static constexpr time_unit_type type = time_unit_type::day; };
    template<> struct map_to_time_type<hours> { static constexpr time_unit_type type = time_unit_type::hour; };
    template<> struct map_to_time_type<minutes> { static constexpr time_unit_type type = time_unit_type::minute; };
    template<> struct map_to_time_type<std::chrono::seconds> { static constexpr time_unit_type type = time_unit_type::second; };
    template<> struct map_to_time_type<std::chrono::milliseconds> { static constexpr time_unit_type type = time_unit_type::millisecond; };
    template<> struct map_to_time_type<filetime_nanoseconds> { static constexpr time_unit_type type = time_unit_type::filetime_nanosecond; };

    enum class string_type
    {
        singular,
        plural
    };

    std::wstring const& get_label_string(time_unit_type time_type, string_type type);
    std::tuple<std::wstring, std::wstring> const& get_label_strings(time_unit_type type);
    time_unit_type get_label_type(std::wstring_view label);

    namespace resources
    {
        std::unordered_map<time_unit_type, std::tuple<std::wstring, std::wstring>> const& get_type_strings();
    }
}
