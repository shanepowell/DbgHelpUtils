#pragma once

namespace MiniDumpExplorerApp
{
    struct SystemTime : winrt::implements<SystemTime, winrt::MiniDumpExplorer::ISystemTime>
    {
        SystemTime(SYSTEMTIME const& system_time);

        [[nodiscard]] uint16_t Year() const;
        [[nodiscard]] uint16_t Month() const;
        [[nodiscard]] uint16_t DayOfWeek() const;
        [[nodiscard]] uint16_t Day() const;
        [[nodiscard]] uint16_t Hour() const;
        [[nodiscard]] uint16_t Minute() const;
        [[nodiscard]] uint16_t Second() const;
        [[nodiscard]] uint16_t Milliseconds() const;

    private:
        SYSTEMTIME const& system_time_;
    };
}

