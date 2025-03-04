#include "handle_operation_list_stream.h"

#include "mini_dump.h"

namespace dlg_help_utils
{
    handle_operation_list_stream::handle_operation_list_stream(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(HandleOperationListStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        auto const* data = dump.rva32(entry->Location);
        handle_operation_list_ = static_cast<MINIDUMP_HANDLE_OPERATION_LIST const*>(data);
        found_ = true;

        if (handle_operation_list_->SizeOfHeader == sizeof(MINIDUMP_HANDLE_OPERATION_LIST)
            && handle_operation_list_->SizeOfEntry == sizeof(AVRF_HANDLE_OPERATION))
        {
            list_ = reinterpret_cast<AVRF_HANDLE_OPERATION const*>(static_cast<uint8_t const*>(data) + sizeof(
                MINIDUMP_HANDLE_OPERATION_LIST));
            is_valid_ = true;
        }
    }

    generator<AVRF_HANDLE_OPERATION const*> handle_operation_list_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        for (size_t index = 0; index < handle_operation_list_->NumberOfEntries; ++index)
        {
            // ReSharper disable once CppAwaiterTypeIsNotClass
            co_yield &list_[index];
        }
    }
}
