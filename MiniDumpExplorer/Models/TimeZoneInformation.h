#pragma once

#include "TimeZoneInformation.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct TimeZoneInformation : TimeZoneInformationT<TimeZoneInformation>, GlobalOptionsNotifyPropertyChangedBase<TimeZoneInformation>
    {
        TimeZoneInformation();

        void Set(TIME_ZONE_INFORMATION const& time_zone_information);

        int32_t Bias() const { return time_zone_information_.Bias; }
        hstring StandardName() const { return standard_name_; }
        hstring StandardDateString() const { return standard_date_; }
        int32_t StandardBias() const { return time_zone_information_.StandardBias; }
        hstring DaylightName() const { return daylight_name_; }
        hstring DaylightDateString() const { return daylight_date_; }
        int32_t DaylightBias() const { return time_zone_information_.DaylightBias; }

    private:
        TIME_ZONE_INFORMATION time_zone_information_{};
        hstring standard_name_;
        hstring daylight_name_;
        hstring standard_date_;
        hstring daylight_date_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct TimeZoneInformation : TimeZoneInformationT<TimeZoneInformation, implementation::TimeZoneInformation>
    {
    };
}
