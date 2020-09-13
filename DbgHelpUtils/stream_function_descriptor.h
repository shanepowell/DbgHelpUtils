#pragma once
#define WIN32_LEAN_AND_MEAN
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>

#include <experimental/generator>

#include "stream_function_entry.h"
#include "function_table_entry_type.h"

namespace dlg_help_utils
{
    class stream_function_descriptor
    {
    public:
        stream_function_descriptor(MINIDUMP_FUNCTION_TABLE_DESCRIPTOR const& header, ULONG32 size_of_native_descriptor,
                                   ULONG32 size_of_function_entry, function_table_entry_type entry_type);

        MINIDUMP_FUNCTION_TABLE_DESCRIPTOR const* operator->() const { return &header_; }
        [[nodiscard]] void const* native_descriptor() const { return native_descriptor_; }
        [[nodiscard]] size_t native_descriptor_size() const { return size_of_native_descriptor_; }
        [[nodiscard]] void const* raw_function_table() const { return function_entry_table_; }

        [[nodiscard]] std::experimental::generator<stream_function_entry> list() const;

    private:
        MINIDUMP_FUNCTION_TABLE_DESCRIPTOR const& header_;
        ULONG32 size_of_native_descriptor_{0};
        ULONG32 size_of_function_entry_{0};
        function_table_entry_type entry_type_{function_table_entry_type::unknown};
        void const* native_descriptor_{nullptr};
        uint8_t const* function_entry_table_{nullptr};
    };
}
