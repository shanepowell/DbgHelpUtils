#include "pch.h"
#include "TimeZoneInformation.h"

#include "SystemTime.h"

namespace MiniDumpExplorerApp
{
    TimeZoneInformation::TimeZoneInformation(TIME_ZONE_INFORMATION const& time_zone)
        : time_zone_{time_zone}
        , standard_name_{time_zone_.StandardName}
        , daylight_name_{time_zone_.DaylightName}
        , standard_date_{*winrt::make_self<SystemTime>(time_zone_.StandardDate)}
        , daylight_date_{*winrt::make_self<SystemTime>(time_zone_.DaylightDate)}
    {
    }

    uint32_t TimeZoneInformation::Bias() const
    {
        return time_zone_.Bias;
    }

    winrt::hstring TimeZoneInformation::StandardName() const
    {
        return standard_name_;
    }

    winrt::MiniDumpExplorer::ISystemTime TimeZoneInformation::StandardDate() const
    {
        return standard_date_;
    }

    uint32_t TimeZoneInformation::StandardBias() const
    {
        return time_zone_.StandardBias;
    }

    winrt::hstring TimeZoneInformation::DaylightName() const
    {
        return daylight_name_;
    }

    winrt::MiniDumpExplorer::ISystemTime TimeZoneInformation::DaylightDate() const
    {
        return daylight_date_;
    }

    uint32_t TimeZoneInformation::DaylightBias() const
    {
        return time_zone_.DaylightBias;
    }
}
