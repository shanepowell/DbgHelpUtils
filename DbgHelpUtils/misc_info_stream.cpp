#include "misc_info_stream.h"

#include "mini_dump.h"
#include "string_stream.h"

#include <chrono>

using namespace std::string_view_literals;

namespace dlg_help_utils
{
    misc_info_stream::misc_info_stream(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(MiscInfoStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        auto const* data = dump.rva32(entry->Location);
        found_ = true;

        if (entry->Location.DataSize >= sizeof(MINIDUMP_MISC_INFO))
        {
            misc_info_version_ = 1;
            misc_info_ = static_cast<MINIDUMP_MISC_INFO const*>(data);
            is_valid_ = true;
        }
        else
        {
            return;
        }

        if (entry->Location.DataSize >= sizeof(MINIDUMP_MISC_INFO_2))
        {
            misc_info_version_ = 2;
            misc_info_2_ = static_cast<MINIDUMP_MISC_INFO_2 const*>(data);
        }

        if (entry->Location.DataSize >= sizeof(MINIDUMP_MISC_INFO_3))
        {
            misc_info_version_ = 3;
            misc_info_3_ = static_cast<MINIDUMP_MISC_INFO_3 const*>(data);
        }

        if (entry->Location.DataSize >= sizeof(MINIDUMP_MISC_INFO_4))
        {
            misc_info_version_ = 4;
            misc_info_4_ = static_cast<MINIDUMP_MISC_INFO_4 const*>(data);
        }

        if (entry->Location.DataSize >= sizeof(MINIDUMP_MISC_INFO_5))
        {
            misc_info_version_ = 5;
            misc_info_5_ = static_cast<MINIDUMP_MISC_INFO_5 const*>(data);
        }
    }

    time_utils::locale_timezone_info misc_info_stream::get_dump_file_timezone_info(mini_dump const& dump, size_t const index)
    {
        time_utils::locale_timezone_info rv;
        misc_info_stream const misc_info{dump, index};

        if (!misc_info.found())
        {
            return rv;
        }

        if (!misc_info.is_valid())
        {
            return rv;
        }

        if (auto const& info = misc_info.misc_info();
            info.Flags1 & MINIDUMP_MISC3_TIMEZONE)
        {
            if(info.Flags1 & MINIDUMP_MISC1_PROCESS_TIMES)
            {
                using namespace std::chrono;
                auto tp = system_clock::from_time_t(info.ProcessCreateTime);
                auto dp = floor<days>(tp);
                year_month_day ymd{dp};
                rv.time_zone_id_year_ = static_cast<int>(ymd.year());
            }
            auto const& info_3 = misc_info.misc_info_3();
            rv.time_zone_id_ = info_3.TimeZoneId;
            rv.time_zone_ = &info_3.TimeZone;
        }

        return rv;
    }
}
