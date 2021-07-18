#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <experimental/generator>

namespace dlg_help_utils
{
    class mini_dump;

    class memory_list_stream
    {
    public:
        explicit memory_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_MEMORY_LIST const& memory_list() const { return *memory_list_; }

        [[nodiscard]] std::experimental::generator<MINIDUMP_MEMORY_DESCRIPTOR const*> list() const;

        [[nodiscard]] void const* find_address_range(uint64_t address, size_t length) const;
        [[nodiscard]] void const* find_any_address_range(uint64_t address, size_t& length) const;

    private:
        mini_dump const& dump_;
        bool found_{false};
        size_t index_;
        MINIDUMP_MEMORY_LIST const* memory_list_{nullptr};
    };
}
