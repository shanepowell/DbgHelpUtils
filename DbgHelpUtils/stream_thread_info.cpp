#include "stream_thread_info.h"

#include "mini_dump.h"
#include "thread_names_list_stream.h"

namespace dlg_help_utils
{
    stream_thread_info::stream_thread_info(MINIDUMP_THREAD_INFO const& thread_info,
                                           thread_names_list_stream const& names_list)
        : thread_info_{thread_info}
          , thread_name_{names_list.get_thread_name_for_thread_id(thread_info.ThreadId)}
    {
    }
}
