#include "pch.h"
#include "GlobalOptionItems.h"

#include "Helpers/GlobalOptions.h"

#if __has_include("GlobalOptionItems.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "GlobalOptionItems.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

using namespace std::chrono_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    wchar_t const * const GlobalOptionItems::DisplayHexadecimalNumericFormatString = L"DisplayHexadecimalNumericFormat";

    wchar_t const * const GlobalOptionItems::SizeNumberDisplayUnitFormatAutoString = L"SizeNumberDisplayUnitFormatAuto";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayUnitFormatBytesString = L"SizeNumberDisplayUnitFormatBytes";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayUnitFormatKilobytesString = L"SizeNumberDisplayUnitFormatKilobytes";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayUnitFormatMegabytesString = L"SizeNumberDisplayUnitFormatMegabytes";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayUnitFormatGigabytesString = L"SizeNumberDisplayUnitFormatGigabytes";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayUnitFormatTerabytesString = L"SizeNumberDisplayUnitFormatTerabytes";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayUnitFormatPetabytesString = L"SizeNumberDisplayUnitFormatPetabytes";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayUnitFormatExabytesString = L"SizeNumberDisplayUnitFormatExabytes";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayUnitFormatRawString = L"SizeNumberDisplayUnitFormatRaw";

    wchar_t const * const GlobalOptionItems::SizeNumberDisplayPrintFormatFullString = L"SizeNumberDisplayPrintFormatFull";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayPrintFormatCompactString = L"SizeNumberDisplayPrintFormatCompact";

    wchar_t const * const GlobalOptionItems::SizeNumberDisplayBase16String = L"SizeNumberDisplayBase16";
    wchar_t const * const GlobalOptionItems::SizeNumberDisplayBase10String = L"SizeNumberDisplayBase10";

    wchar_t const * const GlobalOptionItems::TimeStampLocaleLocalString = L"TimeStampLocaleLocal";
    wchar_t const * const GlobalOptionItems::TimeStampLocaleUtcString = L"TimeStampLocaleUtc";
    wchar_t const * const GlobalOptionItems::TimeStampLocaleDumpFileString = L"TimeStampLocaleDumpFile";
    wchar_t const * const GlobalOptionItems::DateFormatLongDateString = L"DateFormatLongDate";
    wchar_t const * const GlobalOptionItems::DateFormatShortDateString = L"DateFormatShortDate";
    wchar_t const * const GlobalOptionItems::DateFormatUseAltCalendarString = L"DateFormatUseAltCalendar";
    wchar_t const * const GlobalOptionItems::DateFormatYearMonthString = L"DateFormatYearMonth";
    wchar_t const * const GlobalOptionItems::DateFormatMonthDayString = L"DateFormatMonthDay";
    wchar_t const * const GlobalOptionItems::DateFormatLeftToRightReadingString = L"DateFormatLeftToRightReading";
    wchar_t const * const GlobalOptionItems::DateFormatRightToLeftReadingString = L"DateFormatRightToLeftReading";
    wchar_t const * const GlobalOptionItems::DateFormatAutoLayoutString = L"DateFormatAutoLayout";
    wchar_t const * const GlobalOptionItems::TimeFormat24HourString = L"TimeFormat24Hour";
    wchar_t const * const GlobalOptionItems::TimeFormatNoTimeMarkerString = L"TimeFormatNoTimeMarker";
    wchar_t const * const GlobalOptionItems::TimeFormatNoSecondsString = L"TimeFormatNoSeconds";
    wchar_t const * const GlobalOptionItems::TimeFormatNoMinutesOrSecondsString = L"TimeFormatNoMinutesOrSeconds";

    wchar_t const * const GlobalOptionItems::DurationFormatTimeSpanString = L"DurationFormatTimeSpan";
    wchar_t const * const GlobalOptionItems::DurationFormatAutoString = L"DurationFormatAuto";
    wchar_t const * const GlobalOptionItems::DurationFormatMillisecondsString = L"DurationFormatMilliseconds";
    wchar_t const * const GlobalOptionItems::DurationFormatSecondsString = L"DurationFormatSeconds";
    wchar_t const * const GlobalOptionItems::DurationFormatMinutesString = L"DurationFormatMinutes";
    wchar_t const * const GlobalOptionItems::DurationFormatHoursString = L"DurationFormatHours";
    wchar_t const * const GlobalOptionItems::DurationFormatDaysString = L"DurationFormatDays";
    wchar_t const * const GlobalOptionItems::DurationFormatWeeksString = L"DurationFormatWeeks";
    wchar_t const * const GlobalOptionItems::DurationFormatMonthsString = L"DurationFormatMonths";
    wchar_t const * const GlobalOptionItems::DurationFormatYearsString = L"DurationFormatYears";
    wchar_t const * const GlobalOptionItems::DurationFormatRawString = L"DurationFormatRaw";

    GlobalOptionItems::GlobalOptionItems()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                DisplayHexadecimalNumericFormatString
            }, 
            {
                SizeNumberDisplayUnitFormatAutoString,
                SizeNumberDisplayUnitFormatBytesString,
                SizeNumberDisplayUnitFormatKilobytesString,
                SizeNumberDisplayUnitFormatMegabytesString,
                SizeNumberDisplayUnitFormatGigabytesString,
                SizeNumberDisplayUnitFormatTerabytesString,
                SizeNumberDisplayUnitFormatPetabytesString,
                SizeNumberDisplayUnitFormatExabytesString,
                SizeNumberDisplayUnitFormatRawString,

                SizeNumberDisplayPrintFormatFullString,
                SizeNumberDisplayPrintFormatCompactString,

                SizeNumberDisplayBase16String,
                SizeNumberDisplayBase10String
            },
            {
                TimeStampLocaleLocalString,
                TimeStampLocaleUtcString,
                TimeStampLocaleDumpFileString,
                DateFormatLongDateString,
                DateFormatShortDateString,
                TimeFormat24HourString,
                TimeFormatNoTimeMarkerString,
                TimeFormatNoSecondsString,
                TimeFormatNoMinutesOrSecondsString,
            },
            {
                DurationFormatTimeSpanString,
                DurationFormatAutoString,
                DurationFormatMillisecondsString,
                DurationFormatSecondsString,
                DurationFormatMinutesString,
                DurationFormatHoursString,
                DurationFormatDaysString,
                DurationFormatWeeksString,
                DurationFormatMonthsString,
                DurationFormatYearsString,
                DurationFormatRawString
            })
    {
    }

    bool GlobalOptionItems::DisplayHexadecimalNumericFormat()
    {
        return GlobalOptions::Options().NumberDisplayFormat() == NumberDisplayFormatType::Hexadecimal;
    }

    void GlobalOptionItems::DisplayHexadecimalNumericFormat(bool const value)
    {
        auto const numericFormat = value ? NumberDisplayFormatType::Hexadecimal : NumberDisplayFormatType::Decimal;
        GlobalOptions::Options().NumberDisplayFormat(numericFormat);
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatAuto()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormatType::Auto;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatAuto([[maybe_unused]] bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormatType::Auto);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatBytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormatType::Bytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatBytes(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormatType::Bytes);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatKilobytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormatType::Kilobytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatKilobytes(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormatType::Kilobytes);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatMegabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormatType::Megabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatMegabytes(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormatType::Megabytes);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatGigabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormatType::Gigabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatGigabytes(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormatType::Gigabytes);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatTerabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormatType::Terabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatTerabytes(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormatType::Terabytes);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatPetabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormatType::Petabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatPetabytes(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormatType::Petabytes);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatExabytes()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormatType::Exabytes;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatExabytes(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormatType::Exabytes);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayUnitFormatRaw()
    {
        return GlobalOptions::Options().SizeNumberDisplayFormat() == SizeNumberDisplayFormatType::Raw;
    }

    void GlobalOptionItems::SizeNumberDisplayUnitFormatRaw(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeNumberDisplayFormat(SizeNumberDisplayFormatType::Raw);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayPrintFormatFull()
    {
        return GlobalOptions::Options().SizeFormat() == dlg_help_utils::size_units::print::full;
    }

    void GlobalOptionItems::SizeNumberDisplayPrintFormatFull(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeFormat(dlg_help_utils::size_units::print::full);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayPrintFormatCompact()
    {
        return GlobalOptions::Options().SizeFormat() == dlg_help_utils::size_units::print::compact;
    }

    void GlobalOptionItems::SizeNumberDisplayPrintFormatCompact(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeFormat(dlg_help_utils::size_units::print::compact);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayBase16()
    {
        return GlobalOptions::Options().SizeBase() == SizeDisplayNumberBaseType::Base16;
    }

    void GlobalOptionItems::SizeNumberDisplayBase16(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeBase(SizeDisplayNumberBaseType::Base16);
        }
    }

    bool GlobalOptionItems::SizeNumberDisplayBase10()
    {
        return GlobalOptions::Options().SizeBase() == SizeDisplayNumberBaseType::Base10;
    }

    void GlobalOptionItems::SizeNumberDisplayBase10(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().SizeBase(SizeDisplayNumberBaseType::Base10);
        }
    }

    bool GlobalOptionItems::TimeStampLocaleLocal()
    {
        return GlobalOptions::Options().TimeStampLocale() == TimeStampLocaleType::Local;
    }

    void GlobalOptionItems::TimeStampLocaleLocal(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().TimeStampLocale(TimeStampLocaleType::Local);
        }
    }

    bool GlobalOptionItems::TimeStampLocaleUtc()
    {
        return GlobalOptions::Options().TimeStampLocale() == TimeStampLocaleType::UTC;
    }

    void GlobalOptionItems::TimeStampLocaleUtc(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().TimeStampLocale(TimeStampLocaleType::UTC);
        }
    }

    bool GlobalOptionItems::TimeStampLocaleDumpFile()
    {
        return GlobalOptions::Options().TimeStampLocale() == TimeStampLocaleType::DumpFile;
    }

    void GlobalOptionItems::TimeStampLocaleDumpFile(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().TimeStampLocale(TimeStampLocaleType::DumpFile);
        }
    }

    bool GlobalOptionItems::DateFormatLongDate()
    {
        return (GlobalOptions::Options().DateFormatFlags() & DATE_LONGDATE) == DATE_LONGDATE;
    }

    void GlobalOptionItems::DateFormatLongDate(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DateFormatFlags((GlobalOptions::Options().DateFormatFlags() | DATE_LONGDATE) & ~(DATE_SHORTDATE | DATE_MONTHDAY | DATE_YEARMONTH));
        }
    }

    bool GlobalOptionItems::DateFormatShortDate()
    {
        return (GlobalOptions::Options().DateFormatFlags() & DATE_SHORTDATE) == DATE_SHORTDATE;
    }

    void GlobalOptionItems::DateFormatShortDate(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DateFormatFlags((GlobalOptions::Options().DateFormatFlags() | DATE_SHORTDATE) & ~(DATE_LONGDATE | DATE_MONTHDAY | DATE_YEARMONTH));
        }
    }

    bool GlobalOptionItems::DateFormatUseAltCalendar()
    {
        return (GlobalOptions::Options().DateFormatFlags() & DATE_USE_ALT_CALENDAR) == DATE_USE_ALT_CALENDAR;
    }

    void GlobalOptionItems::DateFormatUseAltCalendar(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DateFormatFlags(GlobalOptions::Options().DateFormatFlags() | DATE_USE_ALT_CALENDAR);
        }
        else
        {
            GlobalOptions::Options().DateFormatFlags(GlobalOptions::Options().DateFormatFlags() & ~DATE_USE_ALT_CALENDAR);
        }
    }

    bool GlobalOptionItems::DateFormatYearMonth()
    {
        return (GlobalOptions::Options().DateFormatFlags() & DATE_YEARMONTH) == DATE_YEARMONTH;
    }

    void GlobalOptionItems::DateFormatYearMonth(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DateFormatFlags((GlobalOptions::Options().DateFormatFlags() | DATE_YEARMONTH) & ~(DATE_LONGDATE | DATE_SHORTDATE | DATE_MONTHDAY));
        }
    }

    bool GlobalOptionItems::DateFormatMonthDay()
    {
        return (GlobalOptions::Options().DateFormatFlags() & DATE_MONTHDAY) == DATE_MONTHDAY;
    }

    void GlobalOptionItems::DateFormatMonthDay(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DateFormatFlags((GlobalOptions::Options().DateFormatFlags() | DATE_MONTHDAY) & ~(DATE_LONGDATE | DATE_SHORTDATE | DATE_YEARMONTH));
        }
    }

    bool GlobalOptionItems::DateFormatLeftToRightReading()
    {
        return (GlobalOptions::Options().DateFormatFlags() & DATE_LTRREADING) == DATE_LTRREADING;
    }

    void GlobalOptionItems::DateFormatLeftToRightReading(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DateFormatFlags((GlobalOptions::Options().DateFormatFlags() | DATE_LTRREADING) & ~(DATE_RTLREADING | DATE_AUTOLAYOUT));
        }
    }

    bool GlobalOptionItems::DateFormatRightToLeftReading()
    {
        return (GlobalOptions::Options().DateFormatFlags() & DATE_RTLREADING) == DATE_RTLREADING;
    }

    void GlobalOptionItems::DateFormatRightToLeftReading(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DateFormatFlags((GlobalOptions::Options().DateFormatFlags() | DATE_RTLREADING) & ~(DATE_LTRREADING | DATE_AUTOLAYOUT));
        }
    }

    bool GlobalOptionItems::DateFormatAutoLayout()
    {
        return (GlobalOptions::Options().DateFormatFlags() & DATE_AUTOLAYOUT) == DATE_AUTOLAYOUT;
    }

    void GlobalOptionItems::DateFormatAutoLayout(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DateFormatFlags((GlobalOptions::Options().DateFormatFlags() | DATE_AUTOLAYOUT) & ~(DATE_LTRREADING | DATE_RTLREADING));
        }
    }

    bool GlobalOptionItems::TimeFormat24Hour()
    {
        return (GlobalOptions::Options().TimeFormatFlags() & TIME_FORCE24HOURFORMAT) == TIME_FORCE24HOURFORMAT;
    }

    void GlobalOptionItems::TimeFormat24Hour(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() | TIME_FORCE24HOURFORMAT);
        }
        else
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() & ~TIME_FORCE24HOURFORMAT);
        }
    }

    bool GlobalOptionItems::TimeFormatNoTimeMarker()
    {
        return (GlobalOptions::Options().TimeFormatFlags() & TIME_NOTIMEMARKER) == TIME_NOTIMEMARKER;
    }

    void GlobalOptionItems::TimeFormatNoTimeMarker(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() | TIME_NOTIMEMARKER);
        }
        else
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() & ~TIME_NOTIMEMARKER);
        }
    }

    bool GlobalOptionItems::TimeFormatNoSeconds()
    {
        return (GlobalOptions::Options().TimeFormatFlags() & TIME_NOSECONDS) == TIME_NOSECONDS;
    }

    void GlobalOptionItems::TimeFormatNoSeconds(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() | TIME_NOSECONDS);
        }
        else
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() & ~TIME_NOSECONDS);
        }
    }

    bool GlobalOptionItems::TimeFormatNoMinutesOrSeconds()
    {
        return (GlobalOptions::Options().TimeFormatFlags() & TIME_NOMINUTESORSECONDS) == TIME_NOMINUTESORSECONDS;
    }

    void GlobalOptionItems::TimeFormatNoMinutesOrSeconds(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() | TIME_NOMINUTESORSECONDS);
        }
        else
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() & ~TIME_NOMINUTESORSECONDS);
        }
    }

    bool GlobalOptionItems::DurationFormatTimeSpan()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::TimeSpan;
    }

    void GlobalOptionItems::DurationFormatTimeSpan(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::TimeSpan);
        }
    }

    bool GlobalOptionItems::DurationFormatAuto()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Auto;
    }

    void GlobalOptionItems::DurationFormatAuto(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Auto);
        }
    }

    bool GlobalOptionItems::DurationFormatMilliseconds()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Milliseconds;
    }

    void GlobalOptionItems::DurationFormatMilliseconds(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Milliseconds);
        }
    }

    bool GlobalOptionItems::DurationFormatSeconds()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Seconds;
    }

    void GlobalOptionItems::DurationFormatSeconds(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Seconds);
        }
    }

    bool GlobalOptionItems::DurationFormatMinutes()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Minutes;
    }

    void GlobalOptionItems::DurationFormatMinutes(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Minutes);
        }
    }

    bool GlobalOptionItems::DurationFormatHours()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Hours;
    }

    void GlobalOptionItems::DurationFormatHours(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Hours);
        }
    }

    bool GlobalOptionItems::DurationFormatDays()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Days;
    }

    void GlobalOptionItems::DurationFormatDays(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Days);
        }
    }

    bool GlobalOptionItems::DurationFormatWeeks()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Weeks;
    }

    void GlobalOptionItems::DurationFormatWeeks(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Weeks);
        }
    }

    bool GlobalOptionItems::DurationFormatMonths()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Months;
    }

    void GlobalOptionItems::DurationFormatMonths(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Months);
        }
    }

    bool GlobalOptionItems::DurationFormatYears()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Years;
    }

    void GlobalOptionItems::DurationFormatYears(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Years);
        }
    }

    bool GlobalOptionItems::DurationFormatRaw()
    {
        return GlobalOptions::Options().DurationFormat() == DurationFormatType::Raw;
    }

    void GlobalOptionItems::DurationFormatRaw(bool const value)
    {
        if(value)
        {
            GlobalOptions::Options().DurationFormat(DurationFormatType::Raw);
        }
    }
}
