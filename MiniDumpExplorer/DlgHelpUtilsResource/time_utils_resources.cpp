#include "pch.h"
#include "DbgHelpUtils/time_utils.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/stream_hex_dump.h"
#include "Helpers/GlobalOptions.h"
#include "Utility/logger.h"

using namespace std::string_literals;

namespace dlg_help_utils::time_utils::resources
{
    LCID get_locale_id()
    {
        return GlobalOptions::Options().LocaleId();
    }

    DWORD get_date_format_flags()
    {
        return GlobalOptions::Options().DateFormatFlags();
    }

    void reset_date_format_flags()
    {
        logger::Log().LogMessage(log_level::error, std::format(L"Invalid date format flags: {} - resetting to long date format", stream_hex_dump::to_hex_full(GlobalOptions::Options().DateFormatFlags())));
        GlobalOptions::Options().DateFormatFlags(DATE_LONGDATE);
    }

    std::wstring get_date_format_format()
    {
        return GlobalOptions::Options().DateFormat();
    }

    DWORD get_time_format_flags()
    {
        return GlobalOptions::Options().TimeFormatFlags();
    }

    std::wstring get_time_format_format()
    {
        return GlobalOptions::Options().TimeFormat();
    }

    void reset_time_format_flags()
    {
        logger::Log().LogMessage(log_level::error, std::format(L"Invalid time format flags: {} - resetting to no time format", stream_hex_dump::to_hex_full(GlobalOptions::Options().TimeFormatFlags())));
        GlobalOptions::Options().TimeFormatFlags(0);
    }

    std::wstring get_utc_timezone_name()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        return std::wstring{rm.MainResourceMap().GetValue(L"Resources/TimezoneNameUtc").ValueAsString()};
    }
}