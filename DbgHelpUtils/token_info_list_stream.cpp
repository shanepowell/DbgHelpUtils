#include "token_info_list_stream.h"

#include <format>

#include "mini_dump.h"
#include "stream_hex_dump.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils
{
    token_info_list_stream::token_info_list_stream(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(TokenStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        auto const* data = dump.rva32(entry->Location);
        token_info_list_ = static_cast<MINIDUMP_TOKEN_INFO_LIST const*>(data);
        found_ = true;

        if (token_info_list_->ListHeaderSize == sizeof(MINIDUMP_TOKEN_INFO_LIST)
            && token_info_list_->ElementHeaderSize >= sizeof(MINIDUMP_TOKEN_INFO_HEADER)
            && token_info_list_->TokenListSize == entry->Location.DataSize)
        {
            end_list_ = static_cast<uint8_t const*>(data) + entry->Location.DataSize;
            list_ = reinterpret_cast<MINIDUMP_TOKEN_INFO_HEADER const*>(static_cast<uint8_t const*>(data) + sizeof(MINIDUMP_TOKEN_INFO_LIST));
            is_valid_ = true;
        }
    }

    std::generator<stream_token_info> token_info_list_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        auto const* entry = list_;
        for (size_t index = 0; index < token_info_list_->TokenListEntries; ++index)
        {
            auto const* next_entry = reinterpret_cast<MINIDUMP_TOKEN_INFO_HEADER const*>(reinterpret_cast<uint8_t const*>(entry) + entry->TokenSize);
            if (next_entry > end_list_)
            {
                throw exceptions::wide_runtime_error{std::format(L"token info entry [{0}] at [{1}] end data [{2}] out of range from stream data end [{3}]", index, stream_hex_dump::to_hex(entry), stream_hex_dump::to_hex(next_entry), stream_hex_dump::to_hex(end_list_))};
            }

            // ReSharper disable once CppAwaiterTypeIsNotClass
            co_yield stream_token_info{*entry};

            entry = next_entry;
        }
    }
}
