﻿#pragma once

#include <chrono>
#include <ratio>
#include <string>
#include <ostream>
#include <unordered_map>

namespace dlg_help_utils::time_units
{
    // size_unit TYPEDEFS
    using days = std::chrono::duration<int, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>;
    using weeks = std::chrono::duration<int, std::ratio_multiply<std::ratio<7>, days::period>>;
    using years = std::chrono::duration<int, std::ratio_multiply<std::ratio<146097, 400>, days::period>>;
    using months = std::chrono::duration<int, std::ratio_divide<years::period, std::ratio<12>>>;

    std::wstring to_wstring(std::chrono::milliseconds ms);

    template <typename T, typename R>
    std::wostream& operator<<(std::wostream& os, std::chrono::duration<T, R> time)
    {
        os << to_wstring(time);
        return os;
    }

    namespace time_literals
    {
        constexpr days operator "" _days(const unsigned long long val)
        {
            return (days(val));
        }

        constexpr weeks operator "" _weeks(const unsigned long long val)
        {
            return (weeks(val));
        }

        constexpr months operator "" _months(const unsigned long long val)
        {
            return (months(val));
        }

        constexpr years operator "" _years(const unsigned long long val)
        {
            return (years(val));
        }
    } // namespace time_literals


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
    std::pair<std::wstring, std::wstring> const& get_label_strings(time_unit_type type);
    void set_label_strings(std::unordered_map<time_unit_type, std::pair<std::wstring, std::wstring>> user_string_data);
}
