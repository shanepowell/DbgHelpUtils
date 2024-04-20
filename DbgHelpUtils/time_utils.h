#pragma once
#include "windows_setup.h"
#include <string>
#include <chrono>
#include <cstdint>

namespace dlg_help_utils::time_utils
{
    struct locale_timezone_info
    {
        uint32_t time_zone_id_{TIME_ZONE_ID_INVALID};
        int time_zone_id_year_{};
        TIME_ZONE_INFORMATION const* time_zone_{nullptr};
    };

    std::wstring to_dump_file_timestamp_string(time_t const& timestamp, locale_timezone_info const& locale_info);
    std::wstring to_dump_file_timestamp_string(SYSTEMTIME const& st, locale_timezone_info const& locale_info);
    std::wstring from_dump_file_to_local_timestamp_string(time_t const& timestamp, locale_timezone_info const& locale_info);
    std::wstring from_dump_file_to_local_timestamp_string(SYSTEMTIME const& st, locale_timezone_info const& locale_info);
    std::wstring from_dump_file_to_utc_timestamp_string(time_t const& timestamp, locale_timezone_info const& locale_info);
    std::wstring from_dump_file_to_utc_timestamp_string(SYSTEMTIME const& st, locale_timezone_info const& locale_info);

    std::wstring to_specific_timestamp_string(FILETIME const& ft, locale_timezone_info const& locale_info);
    std::wstring to_local_timestamp_string(FILETIME const& ft);
    std::wstring to_utc_timestamp_string(FILETIME const& ft);

    time_t filetime_to_time_t(FILETIME ft);
    time_t filetime_to_time_t(uint64_t ft);
    uint64_t to_file_time(SYSTEMTIME const& st);
    uint64_t to_file_time(FILETIME const& ft);
    time_t to_time_t(SYSTEMTIME const& st);
    FILETIME time_t_to_filetime(time_t st);
    FILETIME to_utc_from_locale_filetime(SYSTEMTIME const& st, locale_timezone_info const& locale_info);
    FILETIME to_utc_from_locale_filetime(uint64_t st, locale_timezone_info const& locale_info);
    FILETIME to_filetime(uint64_t st);
    SYSTEMTIME filetime_to_system_time(FILETIME const& ft);
    FILETIME system_time_to_filetime(SYSTEMTIME const& st);
    std::chrono::milliseconds duration_to_ms(uint64_t duration); // 100-nanosecond internals to ms

    namespace resources
    {
        LCID get_locale_id();
        DWORD get_date_format_flags();
        void reset_date_format_flags();
        std::wstring get_date_format_format();
        DWORD get_time_format_flags();
        void reset_time_format_flags();
        std::wstring get_time_format_format();
        std::wstring get_utc_timezone_name();
    }
}
