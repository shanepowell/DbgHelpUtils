#include "memory_info_list_stream.h"

#include "mini_dump.h"

namespace dlg_help_utils
{
    memory_info_list_stream::memory_info_list_stream(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(MemoryInfoListStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        auto const* data = dump.rva32(entry->Location);
        memory_info_list_ = static_cast<MINIDUMP_MEMORY_INFO_LIST const*>(data);
        found_ = true;

        if (memory_info_list_->SizeOfHeader == sizeof(MINIDUMP_MEMORY_INFO_LIST))
        {
            if (memory_info_list_->SizeOfEntry == sizeof(MINIDUMP_MEMORY_INFO))
            {
                list_ = reinterpret_cast<MINIDUMP_MEMORY_INFO const*>(static_cast<uint8_t const*>(data) + sizeof(
                    MINIDUMP_MEMORY_INFO_LIST));
                is_valid_ = true;
            }
        }
    }

    std::experimental::generator<MINIDUMP_MEMORY_INFO const*> memory_info_list_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        for (size_t index = 0; index < memory_info_list_->NumberOfEntries; ++index)
        {
            // ReSharper disable once CppAwaiterTypeIsNotClass
            co_yield &list_[index];
        }
    }
}
