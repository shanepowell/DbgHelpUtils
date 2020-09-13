#include "thread_names_list_stream.h"

#include "mini_dump.h"

namespace dlg_help_utils
{
    thread_names_list_stream::thread_names_list_stream(mini_dump const& dump, size_t const index)
        : dump_{dump}
          , index_{index}
    {
        auto const* entry = dump.find_stream_type(ThreadNamesStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        thread_name_list_ = static_cast<MINIDUMP_THREAD_NAME_LIST const*>(dump.rva32(entry->Location));
        found_ = true;
    }

    std::experimental::generator<stream_thread_name> thread_names_list_stream::list() const
    {
        for (size_t index = 0; index < thread_name_list_->NumberOfThreadNames; ++index)
        {
            co_yield stream_thread_name{dump_, thread_name_list_->ThreadNames[index]};
        }
    }

    stream_thread_name thread_names_list_stream::get_thread_name_for_thread_id(uint32_t const thread_id) const
    {
        if (found())
        {
            for (auto const& entry : list())
            {
                if (entry->ThreadId == thread_id)
                {
                    return entry;
                }
            }
        }

        return {};
    }
}
