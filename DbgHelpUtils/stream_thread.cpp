#include "stream_thread.h"


#include "memory64_list_stream.h"
#include "memory_list_stream.h"
#include "mini_dump.h"
#include "thread_names_list_stream.h"

namespace dlg_help_utils
{
    stream_thread::stream_thread(mini_dump const& dump, MINIDUMP_THREAD const& thread,
                                 thread_names_list_stream const& names_list, memory_list_stream const& memory_list,
                                 memory64_list_stream const& memory64_list)
        : thread_{thread}
          , thread_name_{names_list.get_thread_name_for_thread_id(thread.ThreadId)}
    {
        if (thread_.Stack.Memory.Rva != 0)
        {
            stack_ = dump.rva32(thread_.Stack.Memory);
        }
        else if (memory_list.found())
        {
            stack_ = memory_list.find_address_range(thread_.Stack.StartOfMemoryRange, thread_.Stack.Memory.DataSize);
        }
        else if (memory64_list.found())
        {
            stack_ = memory64_list.find_address_range(thread_.Stack.StartOfMemoryRange, thread_.Stack.Memory.DataSize);
        }

        thread_context_ = stream_thread_context{dump, thread_.ThreadContext};
    }
}
