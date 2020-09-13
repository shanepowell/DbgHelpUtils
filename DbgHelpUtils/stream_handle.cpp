#include "stream_handle.h"

#include "mini_dump.h"
#include "string_stream.h"

using namespace std::string_view_literals;

namespace dlg_help_utils
{
    stream_handle::stream_handle(mini_dump const& dump, void const* data, size_t const handle_descriptor_version)
        : dump_{dump}
          , data_{data}
          , handle_descriptor_version_{handle_descriptor_version}
    {
        type_name_ = string_stream::to_string(dump, descriptor().TypeNameRva);
        object_name_ = string_stream::to_string(dump, descriptor().ObjectNameRva);
    }

    std::experimental::generator<stream_handle_object_information> stream_handle::list() const
    {
        if (handle_descriptor_version_ < 2)
        {
            co_return;
        }

        if (descriptor_2().ObjectInfoRva == 0)
        {
            co_return;
        }

        auto const* entry = static_cast<MINIDUMP_HANDLE_OBJECT_INFORMATION const*>(dump_.rva32(
            descriptor_2().ObjectInfoRva));
        co_yield stream_handle_object_information{*entry};

        while (entry->NextInfoRva != 0)
        {
            entry = static_cast<MINIDUMP_HANDLE_OBJECT_INFORMATION const*>(dump_.rva32(entry->NextInfoRva));
            co_yield stream_handle_object_information{*entry};
        }
    }
}
