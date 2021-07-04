#include "system_memory_info_stream.h"

#include "mini_dump.h"
#include "string_stream.h"

using namespace std::string_view_literals;

namespace dlg_help_utils
{
    system_memory_info_stream::system_memory_info_stream(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(SystemMemoryInfoStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        auto const* data = dump.rva32(entry->Location);
        found_ = true;

        if (entry->Location.DataSize >= sizeof(MINIDUMP_SYSTEM_MEMORY_INFO_1))
        {
            system_memory_info_version_ = 1;
            system_memory_info_ = static_cast<MINIDUMP_SYSTEM_MEMORY_INFO_1 const*>(data);
            is_valid_ = true;
        }
    }
}
