#include "time_utils.h"

namespace dlg_help_utils::time_utils::resources
{
    LCID get_locale_id()
    {
        return LOCALE_USER_DEFAULT;
    }

    DWORD get_date_format_flags()
    {
        return DATE_LONGDATE;
    }

    void reset_date_format_flags()
    {
    }

    std::wstring get_date_format_format()
    {
        return {};
    }

    DWORD get_time_format_flags()
    {
        return 0;
    }

    void reset_time_format_flags()
    {
    }

    std::wstring get_time_format_format()
    {
        return {};
    }

    std::wstring get_utc_timezone_name()
    {
        return L"UTC";
    }
}
