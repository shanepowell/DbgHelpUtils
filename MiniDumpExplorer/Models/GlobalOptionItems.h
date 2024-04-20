#pragma once

#include "GlobalOptionItems.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct GlobalOptionItems : GlobalOptionItemsT<GlobalOptionItems>, GlobalOptionsNotifyPropertyChangedBase<GlobalOptionItems>
    {
        GlobalOptionItems();

        static wchar_t const * const DisplayHexadecimalNumericFormatString;
        static wchar_t const * const SizeNumberDisplayUnitFormatAutoString;
        static wchar_t const * const SizeNumberDisplayUnitFormatBytesString;
        static wchar_t const * const SizeNumberDisplayUnitFormatKilobytesString;
        static wchar_t const * const SizeNumberDisplayUnitFormatMegabytesString;
        static wchar_t const * const SizeNumberDisplayUnitFormatGigabytesString;
        static wchar_t const * const SizeNumberDisplayUnitFormatTerabytesString;
        static wchar_t const * const SizeNumberDisplayUnitFormatPetabytesString;
        static wchar_t const * const SizeNumberDisplayUnitFormatExabytesString;
        static wchar_t const * const SizeNumberDisplayUnitFormatRawString;

        static wchar_t const * const SizeNumberDisplayPrintFormatFullString;
        static wchar_t const * const SizeNumberDisplayPrintFormatCompactString;

        static wchar_t const * const SizeNumberDisplayBase16String;
        static wchar_t const * const SizeNumberDisplayBase10String;

        static wchar_t const * const TimeStampLocaleLocalString;
        static wchar_t const * const TimeStampLocaleUtcString;
        static wchar_t const * const TimeStampLocaleDumpFileString;
        static wchar_t const * const DateFormatLongDateString;
        static wchar_t const * const DateFormatShortDateString;
        static wchar_t const * const DateFormatUseAltCalendarString;
        static wchar_t const * const DateFormatYearMonthString;
        static wchar_t const * const DateFormatMonthDayString;
        static wchar_t const * const DateFormatLeftToRightReadingString;
        static wchar_t const * const DateFormatRightToLeftReadingString;
        static wchar_t const * const DateFormatAutoLayoutString;
        static wchar_t const * const TimeFormat24HourString;
        static wchar_t const * const TimeFormatNoTimeMarkerString;
        static wchar_t const * const TimeFormatNoSecondsString;
        static wchar_t const * const TimeFormatNoMinutesOrSecondsString;

        static wchar_t const * const DurationFormatTimeSpanString;
        static wchar_t const * const DurationFormatAutoString;
        static wchar_t const * const DurationFormatMillisecondsString;
        static wchar_t const * const DurationFormatSecondsString;
        static wchar_t const * const DurationFormatMinutesString;
        static wchar_t const * const DurationFormatHoursString;
        static wchar_t const * const DurationFormatDaysString;
        static wchar_t const * const DurationFormatWeeksString;
        static wchar_t const * const DurationFormatMonthsString;
        static wchar_t const * const DurationFormatYearsString;
        static wchar_t const * const DurationFormatRawString;

        static bool DisplayHexadecimalNumericFormat();
        static void DisplayHexadecimalNumericFormat(bool value);

        static bool SizeNumberDisplayUnitFormatAuto();
        static void SizeNumberDisplayUnitFormatAuto(bool value);
        static bool SizeNumberDisplayUnitFormatBytes();
        static void SizeNumberDisplayUnitFormatBytes(bool value);
        static bool SizeNumberDisplayUnitFormatKilobytes();
        static void SizeNumberDisplayUnitFormatKilobytes(bool value);
        static bool SizeNumberDisplayUnitFormatMegabytes();
        static void SizeNumberDisplayUnitFormatMegabytes(bool value);
        static bool SizeNumberDisplayUnitFormatGigabytes();
        static void SizeNumberDisplayUnitFormatGigabytes(bool value);
        static bool SizeNumberDisplayUnitFormatTerabytes();
        static void SizeNumberDisplayUnitFormatTerabytes(bool value);
        static bool SizeNumberDisplayUnitFormatPetabytes();
        static void SizeNumberDisplayUnitFormatPetabytes(bool value);
        static bool SizeNumberDisplayUnitFormatExabytes();
        static void SizeNumberDisplayUnitFormatExabytes(bool value);
        static bool SizeNumberDisplayUnitFormatRaw();
        static void SizeNumberDisplayUnitFormatRaw(bool value);

        static bool SizeNumberDisplayPrintFormatFull();
        static void SizeNumberDisplayPrintFormatFull(bool value);
        static bool SizeNumberDisplayPrintFormatCompact();
        static void SizeNumberDisplayPrintFormatCompact(bool value);

        static bool SizeNumberDisplayBase16();
        static void SizeNumberDisplayBase16(bool value);
        static bool SizeNumberDisplayBase10();
        static void SizeNumberDisplayBase10(bool value);

        static bool TimeStampLocaleLocal();
        static void TimeStampLocaleLocal(bool value);
        static bool TimeStampLocaleUtc();
        static void TimeStampLocaleUtc(bool value);
        static bool TimeStampLocaleDumpFile();
        static void TimeStampLocaleDumpFile(bool value);
        static bool DateFormatLongDate();
        static void DateFormatLongDate(bool value);
        static bool DateFormatShortDate();
        static void DateFormatShortDate(bool value);
        static bool DateFormatUseAltCalendar();
        static void DateFormatUseAltCalendar(bool value);
        static bool DateFormatYearMonth();
        static void DateFormatYearMonth(bool value);
        static bool DateFormatMonthDay();
        static void DateFormatMonthDay(bool value);
        static bool DateFormatLeftToRightReading();
        static void DateFormatLeftToRightReading(bool value);
        static bool DateFormatRightToLeftReading();
        static void DateFormatRightToLeftReading(bool value);
        static bool DateFormatAutoLayout();
        static void DateFormatAutoLayout(bool value);
        static bool TimeFormat24Hour();
        static void TimeFormat24Hour(bool value);
        static bool TimeFormatNoTimeMarker();
        static void TimeFormatNoTimeMarker(bool value);
        static bool TimeFormatNoSeconds();
        static void TimeFormatNoSeconds(bool value);
        static bool TimeFormatNoMinutesOrSeconds();
        static void TimeFormatNoMinutesOrSeconds(bool value);

        static bool DurationFormatTimeSpan();
        static void DurationFormatTimeSpan(bool value);
        static bool DurationFormatAuto();
        static void DurationFormatAuto(bool value);
        static bool DurationFormatMilliseconds();
        static void DurationFormatMilliseconds(bool value);
        static bool DurationFormatSeconds();
        static void DurationFormatSeconds(bool value);
        static bool DurationFormatMinutes();
        static void DurationFormatMinutes(bool value);
        static bool DurationFormatHours();
        static void DurationFormatHours(bool value);
        static bool DurationFormatDays();
        static void DurationFormatDays(bool value);
        static bool DurationFormatWeeks();
        static void DurationFormatWeeks(bool value);
        static bool DurationFormatMonths();
        static void DurationFormatMonths(bool value);
        static bool DurationFormatYears();
        static void DurationFormatYears(bool value);
        static bool DurationFormatRaw();
        static void DurationFormatRaw(bool value);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct GlobalOptionItems : GlobalOptionItemsT<GlobalOptionItems, implementation::GlobalOptionItems>
    {
    };
}
