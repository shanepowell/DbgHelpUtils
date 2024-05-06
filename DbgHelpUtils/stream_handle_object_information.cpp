#include "stream_handle_object_information.h"

namespace dlg_help_utils
{
    stream_handle_object_information::stream_handle_object_information(MINIDUMP_HANDLE_OBJECT_INFORMATION const& info)
        : info_{&info}
    {
    }
}
