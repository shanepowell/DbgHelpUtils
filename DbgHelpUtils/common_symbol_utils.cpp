#include "common_symbol_utils.h"

#include "stream_utils.h"
#include "thread_ex_list_stream.h"
#include "thread_list_stream.h"

namespace dlg_help_utils::common_symbol_utils
{

    std::optional<uint64_t> get_teb_address(mini_dump const& mini_dump, thread_names_list_stream const& names_list, memory_list_stream const& memory_list, memory64_list_stream const& memory64_list)
    {
        if (auto const stream = stream_utils::find_stream_for_type(mini_dump, ThreadListStream); stream.has_value())
        {
            if (thread_list_stream const thread_list{ mini_dump, std::get<size_t>(stream.value()) }; thread_list.found() && thread_list.thread_list().NumberOfThreads > 0)
            {
                if (stream_thread const thread{ mini_dump, thread_list.thread_list().Threads[0], names_list, memory_list, memory64_list }; thread->Teb != 0)
                {
                    return thread->Teb;
                }
            }
        }

        if (auto const stream = stream_utils::find_stream_for_type(mini_dump, ThreadExListStream); stream.has_value())
        {
            if (thread_ex_list_stream const thread_ex_list{ mini_dump, std::get<size_t>(stream.value()) }; thread_ex_list.found() && thread_ex_list.thread_list().NumberOfThreads > 0)
            {
                if (stream_thread_ex const thread{ mini_dump, thread_ex_list.thread_list().Threads[0], names_list }; thread->Teb != 0)
                {
                    return thread->Teb;
                }
            }
        }

        return std::nullopt;
    }

}
