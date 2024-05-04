#include "stream_function_descriptor.h"

#include <cstdint>

namespace dlg_help_utils
{
    stream_function_descriptor::stream_function_descriptor(MINIDUMP_FUNCTION_TABLE_DESCRIPTOR const& header,
                                                           ULONG32 const size_of_native_descriptor,
                                                           ULONG32 const size_of_function_entry,
                                                           function_table_entry_type const entry_type)
        : header_(&header)
        , size_of_native_descriptor_(size_of_native_descriptor)
        , size_of_function_entry_(size_of_function_entry)
        , entry_type_{entry_type}
    {
        auto const* data = reinterpret_cast<uint8_t const*>(header_) + sizeof(*header_);

        native_descriptor_ = data;
        function_entry_table_ = data + size_of_native_descriptor;
    }

    std::experimental::generator<stream_function_entry> stream_function_descriptor::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        for (size_t index = 0; index < header_->EntryCount; ++index)
        {
            // ReSharper disable once CppAwaiterTypeIsNotClass
            co_yield stream_function_entry{
                index, function_entry_table_ + (index * size_of_function_entry_), size_of_function_entry_, entry_type_
            };
        }
    }
}
