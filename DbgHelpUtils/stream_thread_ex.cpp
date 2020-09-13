#include "stream_thread_ex.h"

#include "mini_dump.h"
#include "thread_names_list_stream.h"

namespace dlg_help_utils
{
    stream_thread_ex::stream_thread_ex(mini_dump const& dump, MINIDUMP_THREAD_EX const& thread,
                                       thread_names_list_stream const& names_list)
        : thread_{thread}
          , thread_name_{names_list.get_thread_name_for_thread_id(thread.ThreadId)}
    {
        if (thread_.Stack.Memory.Rva != 0)
        {
            stack_ = dump.rva32(thread_.Stack.Memory);
        }
        if (thread_.BackingStore.Memory.Rva != 0)
        {
            backing_store_ = dump.rva32(thread_.BackingStore.Memory);
        }
        thread_context_ = stream_thread_context{dump, thread_.ThreadContext};
    }
}
