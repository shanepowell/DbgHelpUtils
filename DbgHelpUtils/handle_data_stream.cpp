#include "handle_data_stream.h"

#include "mini_dump.h"

namespace dlg_help_utils
{
    handle_data_stream::handle_data_stream(mini_dump const& dump, size_t const index)
        : dump_{dump}
          , index_{index}
    {
        auto const* entry = dump.find_stream_type(HandleDataStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        auto const* data = dump.rva32(entry->Location);
        handle_data_list_ = static_cast<MINIDUMP_HANDLE_DATA_STREAM const*>(data);
        found_ = true;

        if (handle_data_list_->SizeOfHeader == sizeof(MINIDUMP_HANDLE_DATA_STREAM))
        {
            if (handle_data_list_->SizeOfDescriptor == sizeof(MINIDUMP_HANDLE_DESCRIPTOR))
            {
                handle_descriptor_version_ = 1;
            }
            else if (handle_data_list_->SizeOfDescriptor == sizeof(MINIDUMP_HANDLE_DESCRIPTOR_2))
            {
                handle_descriptor_version_ = 2;
            }
            else
            {
                return;
            }

            list_ = static_cast<uint8_t const*>(data) + sizeof(MINIDUMP_HANDLE_OPERATION_LIST);
            is_valid_ = true;

            for (auto handle : list())
            {
                ++handle_type_totals_[handle.type_name()];
            }
        }
    }

    std::experimental::generator<stream_handle> handle_data_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        if (!is_valid()) co_return;

        for (size_t index = 0; index < handle_data_list_->NumberOfDescriptors; ++index)
        {
            // ReSharper disable once CppAwaiterTypeIsNotClass
            co_yield stream_handle{
                dump_, list_ + (index * handle_data_list_->SizeOfDescriptor), handle_descriptor_version_
            };
        }
    }
}
