#include "pch.h"
#include "SystemTime.h"

namespace MiniDumpExplorerApp
{
    SystemTime::SystemTime(SYSTEMTIME const& system_time)
        : system_time_{system_time}
    {
    }

    uint16_t SystemTime::Year() const
    {
        return system_time_.wYear;
    }

    uint16_t SystemTime::Month() const
    {
        return system_time_.wMonth;
    }

    uint16_t SystemTime::DayOfWeek() const
    {
        return system_time_.wDayOfWeek;
    }

    uint16_t SystemTime::Day() const
    {
        return system_time_.wDay;
    }

    uint16_t SystemTime::Hour() const
    {
        return system_time_.wHour;
    }

    uint16_t SystemTime::Minute() const
    {
        return system_time_.wMinute;
    }

    uint16_t SystemTime::Second() const
    {
        return system_time_.wMilliseconds;
    }

    uint16_t SystemTime::Milliseconds() const
    {
        return system_time_.wMilliseconds;
    }
}
