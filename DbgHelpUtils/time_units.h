#pragma once

#include <chrono>
#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include <ostream>
#include <unordered_map>

namespace dlg_help_utils::time_units
{
    std::wstring to_timespan_wstring(std::chrono::milliseconds ms);
    std::wstring to_wstring(std::chrono::milliseconds ms);
    std::wstring to_milliseconds_wstring(std::chrono::milliseconds ms);
    std::wstring to_seconds_wstring(std::chrono::milliseconds ms);
    std::wstring to_minutes_wstring(std::chrono::milliseconds ms);
    std::wstring to_hours_wstring(std::chrono::milliseconds ms);
    std::wstring to_days_wstring(std::chrono::milliseconds ms);
    std::wstring to_weeks_wstring(std::chrono::milliseconds ms);
    std::wstring to_months_wstring(std::chrono::milliseconds ms);
    std::wstring to_years_wstring(std::chrono::milliseconds ms);

    enum class time_unit_type
    {
        year,
        month,
        week,
        day,
        hour,
        minute,
        second,
        millisecond
    };

    using minutes      = std::chrono::duration<long long, std::ratio<60>>;
    using hours        = std::chrono::duration<long long, std::ratio<3600>>;
    using days         = std::chrono::duration<long long, std::ratio_multiply<std::ratio<24>, hours::period>>;
    using weeks        = std::chrono::duration<long long, std::ratio_multiply<std::ratio<7>, days::period>>;
    using years        = std::chrono::duration<long long, std::ratio_multiply<std::ratio<146097, 400>, days::period>>;
    using months       = std::chrono::duration<long long, std::ratio_divide<years::period, std::ratio<12>>>;

    template<typename T> struct map_to_time_type {};
    template<> struct map_to_time_type<years> { static constexpr time_unit_type type = time_unit_type::year; };
    template<> struct map_to_time_type<months> { static constexpr time_unit_type type = time_unit_type::month; };
    template<> struct map_to_time_type<weeks> { static constexpr time_unit_type type = time_unit_type::week; };
    template<> struct map_to_time_type<days> { static constexpr time_unit_type type = time_unit_type::day; };
    template<> struct map_to_time_type<hours> { static constexpr time_unit_type type = time_unit_type::hour; };
    template<> struct map_to_time_type<minutes> { static constexpr time_unit_type type = time_unit_type::minute; };
    template<> struct map_to_time_type<std::chrono::seconds> { static constexpr time_unit_type type = time_unit_type::second; };
    template<> struct map_to_time_type<std::chrono::milliseconds> { static constexpr time_unit_type type = time_unit_type::millisecond; };

    enum class string_type
    {
        singular,
        plural
    };

    inline const char* to_string(string_type const e)
    {
        switch (e)
        {
        case string_type::singular:
            return "Singular";
        case string_type::plural:
            return "Plural";
        }
        return "unknown";
    }

    std::wstring const& get_label_string(time_unit_type time_type, string_type type);
    std::tuple<std::wstring, std::wstring> const& get_label_strings(time_unit_type type);
    time_unit_type get_label_type(std::wstring_view label);

    namespace resources
    {
        std::unordered_map<time_unit_type, std::tuple<std::wstring, std::wstring>> const& get_type_strings();
    }
}
