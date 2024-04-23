#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <experimental/generator>
#include <limits>

namespace dlg_help_utils
{
    class mini_dump;

    class memory_info_list_stream
    {
    public:
        explicit memory_info_list_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] ULONG64 size() const { return memory_info_list_->NumberOfEntries; }

        [[nodiscard]] std::experimental::generator<MINIDUMP_MEMORY_INFO const*> list() const;

    private:
        bool found_{false};
        bool is_valid_{false};
        size_t index_;
        MINIDUMP_MEMORY_INFO_LIST const* memory_info_list_{nullptr};
        MINIDUMP_MEMORY_INFO const* list_{nullptr};
    };
}
