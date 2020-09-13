#include "find_thread_stack.h"

#include "memory64_list_stream.h"
#include "memory_list_stream.h"
#include "thread_ex_list_stream.h"
#include "thread_list_stream.h"
#include "thread_names_list_stream.h"

namespace dlg_help_utils
{
    std::optional<thread_stack> find_thread_stack(mini_dump const& mini_dump, uint32_t const thread_id)
    {
        thread_list_stream const thread_list{mini_dump};
        thread_ex_list_stream const thread_ex_list{mini_dump};
        thread_names_list_stream const names_list{mini_dump};

        if (thread_list.found())
        {
            memory_list_stream const memory_list{mini_dump};
            memory64_list_stream const memory64_list{mini_dump};
            auto const thread = thread_list.find_thread(thread_id, names_list, memory_list, memory64_list);
            if (thread.has_value())
            {
                return thread_stack{
                    thread->stack(), (*thread)->Stack.Memory.DataSize, (*thread)->Stack.StartOfMemoryRange
                };
            }
        }

        if (thread_ex_list.found())
        {
            auto const thread = thread_ex_list.find_thread(thread_id, names_list);
            if (thread.has_value())
            {
                return thread_stack{
                    thread->stack(), (*thread)->Stack.Memory.DataSize, (*thread)->Stack.StartOfMemoryRange
                };
            }
        }

        return std::nullopt;
    }
}
