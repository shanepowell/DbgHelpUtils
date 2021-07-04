#include "misc_info_stream.h"

#include "mini_dump.h"
#include "string_stream.h"

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
}
