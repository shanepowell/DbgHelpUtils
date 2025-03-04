#include "thread_ex_list_stream.h"

#include "mini_dump.h"
#include "thread_names_list_stream.h"

namespace dlg_help_utils
{
    thread_ex_list_stream::thread_ex_list_stream(mini_dump const& dump, size_t const index)
        : dump_{&dump}
        , index_{index}
    {
        auto const* entry = dump.find_stream_type(ThreadListStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        thread_list_ = static_cast<MINIDUMP_THREAD_EX_LIST const*>(dump.rva32(entry->Location));
        found_ = true;
    }

    generator<stream_thread_ex> thread_ex_list_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        thread_names_list_stream const names_list{*dump_};
        for (size_t index = 0; index < thread_list_->NumberOfThreads; ++index)
        {
            co_yield stream_thread_ex{*dump_, thread_list_->Threads[index], names_list};
        }
    }

    std::optional<stream_thread_ex> thread_ex_list_stream::find_thread(ULONG32 const thread_id,
                                                                       thread_names_list_stream const& names_list) const
    {
        for (size_t index = 0; index < thread_list_->NumberOfThreads; ++index)
        {
            if (thread_list_->Threads[index].ThreadId == thread_id)
            {
                return stream_thread_ex{*dump_, thread_list_->Threads[index], names_list};
            }
        }

        return std::nullopt;
    }
}
