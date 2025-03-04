#include "function_table_stream.h"

#include <format>

#include "mini_dump.h"
#include "stream_hex_dump.h"
#include "system_info_stream.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils
{
    function_table_stream::function_table_stream(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(FunctionTableStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        auto const* data = dump.rva32(entry->Location);
        function_table_list_ = static_cast<MINIDUMP_FUNCTION_TABLE_STREAM const*>(data);
        found_ = true;

        if (function_table_list_->SizeOfHeader == sizeof(MINIDUMP_FUNCTION_TABLE_STREAM)
            && function_table_list_->SizeOfDescriptor >= sizeof(MINIDUMP_FUNCTION_TABLE_DESCRIPTOR))
        {
            end_list_ = static_cast<uint8_t const*>(data) + entry->Location.DataSize;
            list_ = reinterpret_cast<MINIDUMP_FUNCTION_TABLE_DESCRIPTOR const*>(static_cast<uint8_t const*>(data) +
                sizeof(MINIDUMP_FUNCTION_TABLE_STREAM) + function_table_list_->SizeOfAlignPad);
            is_valid_ = true;

            if (system_info_stream const system_info{dump}; system_info.is_x86() || system_info.is_x64())
            {
                if (function_table_list_->SizeOfFunctionEntry == SIZEOF_RFPO_DATA)
                {
                    entry_type_ = function_table_entry_type::fpo_data;
                }
                else if (function_table_list_->SizeOfFunctionEntry == sizeof(IMAGE_FUNCTION_ENTRY))
                {
                    entry_type_ = function_table_entry_type::image_function_entry;
                }
                else if (function_table_list_->SizeOfFunctionEntry == sizeof(IMAGE_FUNCTION_ENTRY64))
                {
                    entry_type_ = function_table_entry_type::image_function_entry_64;
                }
            }
        }
    }

    generator<stream_function_descriptor> function_table_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        auto const* entry = list_;
        for (size_t index = 0; index < function_table_list_->NumberOfDescriptors; ++index)
        {
            auto const* next_entry = reinterpret_cast<MINIDUMP_FUNCTION_TABLE_DESCRIPTOR const*>(reinterpret_cast<
                    uint8_t const*>(entry) + sizeof(MINIDUMP_FUNCTION_TABLE_DESCRIPTOR) + function_table_list_->
                SizeOfNativeDescriptor + (static_cast<size_t>(entry->EntryCount) * function_table_list_->
                    SizeOfFunctionEntry
                ) + entry->SizeOfAlignPad);
            if (next_entry > end_list_)
            {
                throw exceptions::wide_runtime_error{std::format(L"function table entry [{0}] at [{1}] end data [{2}] out of range from stream data end [{3}]", index, stream_hex_dump::to_hex(entry), stream_hex_dump::to_hex(next_entry), stream_hex_dump::to_hex(end_list_))};
            }

            // ReSharper disable once CppAwaiterTypeIsNotClass
            co_yield stream_function_descriptor{
                *entry, function_table_list_->SizeOfNativeDescriptor, function_table_list_->SizeOfFunctionEntry,
                entry_type()
            };

            entry = next_entry;
        }
    }
}
