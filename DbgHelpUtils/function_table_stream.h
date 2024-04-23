#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <experimental/generator>
#include <limits>

#include "stream_function_descriptor.h"
#include "function_table_entry_type.h"

namespace dlg_help_utils
{
    class mini_dump;

    class function_table_stream
    {
    public:
        explicit function_table_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] function_table_entry_type entry_type() const { return entry_type_; }
        [[nodiscard]] size_t index() const { return index_; }
        MINIDUMP_FUNCTION_TABLE_STREAM const* operator->() const { return function_table_list_; }
        [[nodiscard]] ULONG64 size() const { return function_table_list_->NumberOfDescriptors; }

        [[nodiscard]] std::experimental::generator<stream_function_descriptor> list() const;

    private:
        bool found_{false};
        bool is_valid_{false};
        size_t index_;
        void const* end_list_{nullptr};
        MINIDUMP_FUNCTION_TABLE_STREAM const* function_table_list_{nullptr};
        MINIDUMP_FUNCTION_TABLE_DESCRIPTOR const* list_{nullptr};
        function_table_entry_type entry_type_{function_table_entry_type::unknown};
    };
}
