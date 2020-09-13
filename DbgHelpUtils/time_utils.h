#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <chrono>

namespace dlg_help_utils::time_utils
{
    std::wstring to_local_time(time_t timestamp);
    std::wstring to_utc_time(time_t timestamp);
    time_t filetime_to_time_t(FILETIME ft);
    time_t filetime_to_time_t(uint64_t ft);
    std::chrono::milliseconds duration_to_ms(uint64_t duration); // 100-nanosecond internals to ms
}
