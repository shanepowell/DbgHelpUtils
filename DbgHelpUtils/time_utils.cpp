#include "time_utils.h"

#include <iomanip>
#include <sstream>
#include <ctime>

namespace dlg_help_utils::time_utils
{
    std::wstring to_local_time(time_t const timestamp)
    {
        tm local_tm{};
        localtime_s(&local_tm, &timestamp);

        std::wostringstream oss;
        oss << std::put_time(&local_tm, L"%c %Z");
        return std::move(oss).str();
    }

    std::wstring to_utc_time(time_t const timestamp)
    {
        tm utc_tm{};
        gmtime_s(&utc_tm, &timestamp);
        std::wostringstream oss;
        oss << std::put_time(&utc_tm, L"%c");
        return std::move(oss).str();
    }

    time_t filetime_to_time_t(FILETIME const ft)
    {
        ULARGE_INTEGER ull;
        ull.LowPart = ft.dwLowDateTime;
        ull.HighPart = ft.dwHighDateTime;
        return filetime_to_time_t(ull.QuadPart);
    }

    time_t filetime_to_time_t(uint64_t const ft)
    {
        return static_cast<time_t>(ft / 10000000ULL - 11644473600ULL);
    }

    std::chrono::milliseconds duration_to_ms(uint64_t const duration)
    {
        return std::chrono::milliseconds{duration / 10000ULL};
    }
}
