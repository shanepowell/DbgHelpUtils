#include "time_utils.h"

#include "time_units.h"
#include "wide_runtime_error.h"
#include "windows_error.h"

namespace dlg_help_utils::time_utils
{
    namespace 
    {
        time_t filetime_to_time_t(uint64_t const ft)
        {
            return static_cast<time_t>(ft / 10000000ULL - 11644473600ULL);
        }

        time_t filetime_to_time_t(FILETIME const ft)
        {
            ULARGE_INTEGER ull;
            ull.LowPart = ft.dwLowDateTime;
            ull.HighPart = ft.dwHighDateTime;
            return filetime_to_time_t(ull.QuadPart);
        }

        std::wstring to_unknown_timezone_name(TIME_ZONE_INFORMATION const& tzi)
        {
            if(tzi.Bias == 0)
            {
                return resources::get_utc_timezone_name();
            }

            auto minutes = std::chrono::minutes{tzi.Bias};
            auto const hours = std::chrono::duration_cast<std::chrono::hours>(minutes);
            minutes -= hours;
            if(minutes.count() > 0)
            {
                return std::format(L"{0}{1:+03}:{2:02}", resources::get_utc_timezone_name(), hours.count(), std::abs(minutes.count()));
            }

            return std::format(L"{0}{1:+03}", resources::get_utc_timezone_name(), hours.count());
        }

        std::wstring to_timezone_name(TIME_ZONE_INFORMATION const& tzi, SYSTEMTIME const& stLocal, SYSTEMTIME const& stUtc)
        {
            auto const ftUtc = system_time_to_filetime(stUtc);
            auto const ftLocal = system_time_to_filetime(stLocal);

            auto const timeUtc = filetime_to_time_t(ftUtc);
            auto const timeLocal = filetime_to_time_t(ftLocal);
            auto const bias = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::seconds{timeUtc - timeLocal});

            if(bias.count() == tzi.Bias + tzi.DaylightBias)
            {
                return tzi.DaylightName;
            }

            if(bias.count() == tzi.Bias + tzi.StandardBias)
            {
                return tzi.StandardName;
            }

            return to_unknown_timezone_name(tzi);
        }

        std::wstring to_timezone_name(std::wstring_view const name, TIME_ZONE_INFORMATION const& info)
        {
            if(name.empty())
            {
                return to_unknown_timezone_name(info);
            }

            return std::wstring{name};
        }

        int calculate_timezone_id(SYSTEMTIME const& st, SYSTEMTIME standardDate, SYSTEMTIME daylightDate)
        {
            if(standardDate.wYear == 0)
            {
                standardDate.wYear = st.wYear;
            }

            if(daylightDate.wYear == 0)
            {
                daylightDate.wYear = st.wYear;
            }

            auto const ft = to_file_time(system_time_to_filetime(st));
            auto const standardDateFt = to_file_time(system_time_to_filetime(standardDate));

            if(auto const daylightDateFt = to_file_time(system_time_to_filetime(daylightDate));
                standardDateFt > daylightDateFt)
            {
                if(ft >= standardDateFt)
                {
                    return TIME_ZONE_ID_STANDARD;
                }

                if(ft >= daylightDateFt)
                {
                    return TIME_ZONE_ID_DAYLIGHT;
                }

                return TIME_ZONE_ID_STANDARD;
            }
            else
            {
                if(ft >= daylightDateFt)
                {
                    return TIME_ZONE_ID_DAYLIGHT;
                }

                if(ft >= standardDateFt)
                {
                    return TIME_ZONE_ID_STANDARD;
                }

                return TIME_ZONE_ID_DAYLIGHT;
            }
        }

        int calculate_timezone_id(SYSTEMTIME const& st, TIME_ZONE_INFORMATION const& tzi)
        {
            // can't trust the timezone id, so *try* to calculate it based on the supplied TZI information (assuming it hasn't changed over time)
            if((tzi.StandardDate.wYear != 0 && tzi.DaylightDate.wYear != 0) ||
                tzi.DaylightDate.wMonth != 0)
            {
                // strict date switchover
                return calculate_timezone_id(st, tzi.StandardDate, tzi.DaylightDate);
            }

            if(tzi.StandardDate.wYear != 0)
            {
                // strict date switchover
                auto const ft = to_file_time(system_time_to_filetime(st));

                if(auto const standardDateFt = to_file_time(system_time_to_filetime(tzi.StandardDate));
                    ft >= standardDateFt)
                {
                    return TIME_ZONE_ID_STANDARD;
                }

                return TIME_ZONE_ID_UNKNOWN;
            }

            auto const ft = to_file_time(system_time_to_filetime(st));

            auto sd = tzi.StandardDate;
            sd.wYear = st.wYear;
            if(auto const standardDateFt = to_file_time(system_time_to_filetime(sd));
                ft >= standardDateFt)
            {
                return TIME_ZONE_ID_STANDARD;
            }

            return TIME_ZONE_ID_UNKNOWN;
        }

        std::wstring to_timezone_name(SYSTEMTIME const& st, locale_timezone_info const& locale_info)
        {
            TIME_ZONE_INFORMATION localTzi;
            TIME_ZONE_INFORMATION const* st_time_zone = locale_info.time_zone_;
            if(locale_info.time_zone_ == nullptr)
            {
                if (!GetTimeZoneInformationForYear(st.wYear, nullptr, &localTzi ))
                {
                    windows_error::throw_windows_api_error(L"GetTimeZoneInformationForYear");
                }
                st_time_zone = &localTzi;
            }

            switch(locale_info.time_zone_id_year_ == st.wYear ? locale_info.time_zone_id_ : calculate_timezone_id(st, *locale_info.time_zone_))
            {
            case TIME_ZONE_ID_DAYLIGHT:
                return to_timezone_name(st_time_zone->DaylightName, *st_time_zone);

            case TIME_ZONE_ID_STANDARD:
                return to_timezone_name(st_time_zone->StandardName, *st_time_zone);

            default:
            case TIME_ZONE_ID_UNKNOWN:
                return to_unknown_timezone_name(*st_time_zone);
            }
        }

        auto get_timezone_offset_minutes(SYSTEMTIME const& st, locale_timezone_info const& locale_info)
        {
            if(locale_info.time_zone_ == nullptr)
            {
                throw exceptions::wide_runtime_error{L"time zone information can't be null"};
            }

            switch(locale_info.time_zone_id_year_ == st.wYear ? locale_info.time_zone_id_ : calculate_timezone_id(st, *locale_info.time_zone_))
            {
            case TIME_ZONE_ID_DAYLIGHT:
                return locale_info.time_zone_->Bias + locale_info.time_zone_->DaylightBias;

            case TIME_ZONE_ID_STANDARD:
                return locale_info.time_zone_->Bias + locale_info.time_zone_->StandardBias;

            default:
            case TIME_ZONE_ID_UNKNOWN:
                return locale_info.time_zone_->Bias;
            }
        }

        std::wstring get_locale_date_time(SYSTEMTIME const& st, std::wstring_view const time_zone_name)
        {
            auto const date_format{resources::get_date_format_format()};
            auto const* date_format_ptr = date_format.empty() ? nullptr : date_format.data();
            auto const time_format{resources::get_time_format_format()};
            auto const* time_format_ptr = time_format.empty() ? nullptr : time_format.data();

            auto number_of_date_chars = GetDateFormatW(resources::get_locale_id(), resources::get_date_format_flags(), &st, date_format_ptr, nullptr, 0);
            if (number_of_date_chars == 0)
            {
                if(GetLastError() == ERROR_INVALID_FLAGS)
                {
                    // reset flags
                    resources::reset_date_format_flags();

                    number_of_date_chars = GetDateFormatW(resources::get_locale_id(), resources::get_date_format_flags(), &st, date_format_ptr, nullptr, 0);
                }
            }

            if (number_of_date_chars == 0)
            {
                number_of_date_chars = GetDateFormatW(resources::get_locale_id(), resources::get_date_format_flags(), &st, date_format_ptr, nullptr, 0);
            }

            auto number_of_time_chars = GetTimeFormatW(resources::get_locale_id(), resources::get_time_format_flags(), &st, time_format_ptr, nullptr, 0);
            if (number_of_date_chars == 0)
            {
                if(GetLastError() == ERROR_INVALID_FLAGS)
                {
                    // reset flags
                    resources::reset_time_format_flags();

                    number_of_time_chars = GetTimeFormatW(resources::get_locale_id(), resources::get_time_format_flags(), &st, time_format_ptr, nullptr, 0);
                }
            }

            if (number_of_time_chars == 0)
            {
                windows_error::throw_windows_api_error(L"GetTimeFormatW");
            }

            std::wstring result;
            result.resize(number_of_date_chars + number_of_time_chars + time_zone_name.size(), L' ');

            number_of_date_chars = GetDateFormatW(resources::get_locale_id(), resources::get_date_format_flags(), &st, date_format_ptr, result.data(), number_of_date_chars);
            if (number_of_date_chars == 0)
            {
                windows_error::throw_windows_api_error(L"GetDateFormatW");
            }
            result[number_of_date_chars - 1] = L' ';

            number_of_time_chars = GetTimeFormatW(resources::get_locale_id(), resources::get_time_format_flags(), &st, time_format_ptr, result.data() + number_of_date_chars, number_of_time_chars);
            if (number_of_time_chars == 0)
            {
                windows_error::throw_windows_api_error(L"GetTimeFormatW");
            }

            result[number_of_date_chars + number_of_time_chars - 1] = L' ';

            std::ranges::copy(time_zone_name, result.begin() + number_of_date_chars + number_of_time_chars);

            return result;
        }
    }

    std::wstring to_dump_file_timestamp_string(time_t const& timestamp, locale_timezone_info const& locale_info)
    {
        return to_dump_file_timestamp_string(filetime_to_system_time(time_t_to_filetime(timestamp)), locale_info);
    }

    std::wstring to_dump_file_timestamp_string(SYSTEMTIME const& st, locale_timezone_info const& locale_info)
    {
        if(locale_info.time_zone_ != nullptr)
        {
            return get_locale_date_time(st, to_timezone_name(st, locale_info));
        }

        TIME_ZONE_INFORMATION localTzi{};
        if (!GetTimeZoneInformationForYear( st.wYear, nullptr, &localTzi ))
        {
            windows_error::throw_windows_api_error(L"GetTimeZoneInformationForYear");
        }

        SYSTEMTIME utcSt;
        if(!TzSpecificLocalTimeToSystemTime(&localTzi, &st, &utcSt))
        {
            windows_error::throw_windows_api_error(L"TzSpecificLocalTimeToSystemTime");
        }

        return get_locale_date_time(st, to_timezone_name(localTzi, st, utcSt));
    }

    std::wstring from_dump_file_to_local_timestamp_string(time_t const& timestamp, locale_timezone_info const& locale_info)
    {
        if(timestamp == 0)
        {
            return {};
        }

        return from_dump_file_to_local_timestamp_string(filetime_to_system_time(time_t_to_filetime(timestamp)), locale_info);
    }

    std::wstring from_dump_file_to_local_timestamp_string(SYSTEMTIME const& st, locale_timezone_info const& locale_info)
    {
        TIME_ZONE_INFORMATION localTzi{};
        if (!GetTimeZoneInformationForYear( st.wYear, nullptr, &localTzi ))
        {
            windows_error::throw_windows_api_error(L"GetTimeZoneInformationForYear");
        }

        SYSTEMTIME localSt;
        SYSTEMTIME const* useLocalSt;
        SYSTEMTIME utcSt;
        if(locale_info.time_zone_ != nullptr)
        {
            // convert to utc from the specific timezone (manually using timezone_id)
            utcSt = filetime_to_system_time(to_utc_from_locale_filetime(st, locale_info));

            // convert to local time from utc timezone
            if(!SystemTimeToTzSpecificLocalTime(&localTzi, &utcSt, &localSt))
            {
                windows_error::throw_windows_api_error(L"SystemTimeToTzSpecificLocalTime");
            }
            useLocalSt = &localSt;
        }
        else
        {
            // assume already in the local timezone
            if(!TzSpecificLocalTimeToSystemTime(&localTzi, &st, &utcSt))
            {
                windows_error::throw_windows_api_error(L"TzSpecificLocalTimeToSystemTime");
            }

            useLocalSt = &st;
        }

        return get_locale_date_time(st, to_timezone_name(localTzi, *useLocalSt, utcSt));
    }

    std::wstring from_dump_file_to_utc_timestamp_string(time_t const& timestamp, locale_timezone_info const& locale_info)
    {
        if(timestamp == 0)
        {
            return {};
        }

        return from_dump_file_to_utc_timestamp_string(filetime_to_system_time(time_t_to_filetime(timestamp)), locale_info);
    }

    std::wstring from_dump_file_to_utc_timestamp_string(SYSTEMTIME const& st, locale_timezone_info const& locale_info)
    {
        SYSTEMTIME utcSt;
        if(locale_info.time_zone_ != nullptr)
        {
            // convert to utc from the specific timezone (manually using timezone_id)
            utcSt = filetime_to_system_time(to_utc_from_locale_filetime(st, locale_info));
        }
        else
        {
            TIME_ZONE_INFORMATION localTzi{};
            if (!GetTimeZoneInformationForYear( st.wYear, nullptr, &localTzi ))
            {
                windows_error::throw_windows_api_error(L"GetTimeZoneInformationForYear");
            }

            // assume already in the local timezone
            if(!TzSpecificLocalTimeToSystemTime(&localTzi, &st, &utcSt))
            {
                windows_error::throw_windows_api_error(L"TzSpecificLocalTimeToSystemTime");
            }
        }

        return get_locale_date_time(utcSt, resources::get_utc_timezone_name());
    }

    std::wstring to_specific_timestamp_string(FILETIME const& ft, locale_timezone_info const& locale_info)
    {
        if(locale_info.time_zone_ == nullptr)
        {
            return to_local_timestamp_string(ft);
        }

        ULARGE_INTEGER time_value;
        time_value.HighPart = ft.dwHighDateTime;
        time_value.LowPart = ft.dwLowDateTime;

        auto ftLocal = to_filetime(static_cast<uint64_t>(static_cast<int64_t>(time_value.QuadPart) +
            (get_timezone_offset_minutes(
                filetime_to_system_time(
                    to_filetime(
                        time_value.QuadPart + locale_info.time_zone_->Bias)),
                locale_info) 
                * 600000000LL)));

        auto const localSt = filetime_to_system_time(ftLocal);
        return get_locale_date_time(localSt, to_timezone_name(localSt, locale_info));
    }

    std::wstring to_local_timestamp_string(FILETIME const& ft)
    {
        auto const utcSt = filetime_to_system_time(ft);

        TIME_ZONE_INFORMATION localTzi{};
        if (!GetTimeZoneInformationForYear( utcSt.wYear, nullptr, &localTzi ))
        {
            windows_error::throw_windows_api_error(L"GetTimeZoneInformationForYear");
        }

        SYSTEMTIME localSt;
        if(!SystemTimeToTzSpecificLocalTime(&localTzi, &utcSt, &localSt))
        {
            windows_error::throw_windows_api_error(L"SystemTimeToTzSpecificLocalTime");
        }

        return get_locale_date_time(localSt, to_timezone_name(localTzi, localSt, utcSt));
    }

    std::wstring to_utc_timestamp_string(FILETIME const& ft)
    {
        return get_locale_date_time(filetime_to_system_time(ft), resources::get_utc_timezone_name());
    }

    uint64_t to_file_time(SYSTEMTIME const& st)
    {
        return to_file_time(system_time_to_filetime(st));
    }

    uint64_t to_file_time(FILETIME const& ft)
    {
        return (static_cast<uint64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    }

    time_t to_time_t(SYSTEMTIME const& st)
    {
        return filetime_to_time_t(to_file_time(st));
    }

    FILETIME time_t_to_filetime(time_t const st)
    {
        ULARGE_INTEGER time_value;
        time_value.QuadPart = (st * 10000000LL) + 116444736000000000LL;
        FILETIME ft;
        ft.dwLowDateTime = time_value.LowPart;
        ft.dwHighDateTime = time_value.HighPart;
        return ft;
    }

    FILETIME to_utc_from_locale_filetime(SYSTEMTIME const& st, locale_timezone_info const& locale_info)
    {
        return to_filetime(static_cast<uint64_t>(static_cast<int64_t>(to_file_time(st)) - (get_timezone_offset_minutes(st, locale_info) * 600000000LL)));
    }

    FILETIME to_utc_from_locale_filetime(uint64_t const st, locale_timezone_info const& locale_info)
    {
        return to_filetime(static_cast<uint64_t>(static_cast<int64_t>(st)
            - (get_timezone_offset_minutes(
                filetime_to_system_time(
                    to_filetime(st)),
                locale_info)
                * 600000000LL)));
    }

    FILETIME to_filetime(uint64_t const st)
    {
        ULARGE_INTEGER time_value;
        time_value.QuadPart = st;
        FILETIME ft;
        ft.dwLowDateTime = time_value.LowPart;
        ft.dwHighDateTime = time_value.HighPart;
        return ft;
    }

    SYSTEMTIME filetime_to_system_time(FILETIME const& ft)
    {
        SYSTEMTIME st;
        if (!FileTimeToSystemTime(&ft, &st))
        {
            windows_error::throw_windows_api_error(L"FileTimeToSystemTime");
        }

        return st;
    }

    FILETIME system_time_to_filetime(SYSTEMTIME const& st)
    {
        FILETIME ft;
        if (!SystemTimeToFileTime(&st, &ft))
        {
            windows_error::throw_windows_api_error(L"SystemTimeToFileTime");
        }

        return ft;
    }
}
