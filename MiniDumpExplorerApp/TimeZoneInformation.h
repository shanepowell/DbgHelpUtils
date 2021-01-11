#pragma once

namespace MiniDumpExplorerApp
{
    struct TimeZoneInformation : winrt::implements<TimeZoneInformation, winrt::MiniDumpExplorer::ITimeZoneInformation>
    {
        TimeZoneInformation(TIME_ZONE_INFORMATION const& time_zone);

        [[nodiscard]] uint32_t Bias() const;
        [[nodiscard]] winrt::hstring StandardName() const;
        [[nodiscard]] winrt::MiniDumpExplorer::ISystemTime StandardDate() const;
        [[nodiscard]] uint32_t StandardBias() const;
        [[nodiscard]] winrt::hstring DaylightName() const;
        [[nodiscard]] winrt::MiniDumpExplorer::ISystemTime DaylightDate() const;
        [[nodiscard]] uint32_t DaylightBias() const;

    private:
        TIME_ZONE_INFORMATION const& time_zone_;
        winrt::hstring standard_name_;
        winrt::hstring daylight_name_;
        winrt::MiniDumpExplorer::ISystemTime standard_date_;
        winrt::MiniDumpExplorer::ISystemTime daylight_date_;
    };
}
