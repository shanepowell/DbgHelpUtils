#include "thread_info_list_stream.h"

#include "mini_dump.h"
#include "thread_names_list_stream.h"

namespace dlg_help_utils
{
    thread_info_list_stream::thread_info_list_stream(mini_dump const& dump, size_t const index)
        : dump_{dump}
          , index_{index}
    {
        auto const* entry = dump.find_stream_type(ThreadInfoListStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        auto const* data = dump.rva32(entry->Location);
        thread_info_list_ = static_cast<MINIDUMP_THREAD_INFO_LIST const*>(data);
        found_ = true;

        if (thread_info_list_->SizeOfHeader == sizeof(MINIDUMP_THREAD_INFO_LIST))
        {
            if (thread_info_list_->SizeOfEntry == sizeof(MINIDUMP_THREAD_INFO))
            {
                list_ = reinterpret_cast<MINIDUMP_THREAD_INFO const*>(static_cast<uint8_t const*>(data) + sizeof(
                    MINIDUMP_THREAD_INFO_LIST));
                is_valid_ = true;
            }
        }
    }

    std::experimental::generator<stream_thread_info> thread_info_list_stream::list() const
    {
        thread_names_list_stream const names_list{dump_};
        for (size_t index = 0; index < thread_info_list_->NumberOfEntries; ++index)
        {
            // ReSharper disable once CppAwaiterTypeIsNotClass
            co_yield stream_thread_info{list_[index], names_list};
        }
    }
}
