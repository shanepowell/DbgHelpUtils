#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>
#include <experimental/generator>

namespace dlg_help_utils
{
    class mini_dump;

    struct memory64_entry
    {
        ULONG64 start_of_memory_range;
        MINIDUMP_LOCATION_DESCRIPTOR64 location;
    };

    class memory64_list_stream
    {
    public:
        explicit memory64_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_MEMORY64_LIST const& memory_list() const { return *memory_list_; }

        [[nodiscard]] std::experimental::generator<memory64_entry> list() const;

        [[nodiscard]] void const* find_address_range(uint64_t address, size_t length) const;
        [[nodiscard]] void const* find_any_address_range(uint64_t address, size_t& length) const;

    private:
        mini_dump const& dump_;
        bool found_{false};
        size_t index_;
        MINIDUMP_MEMORY64_LIST const* memory_list_{nullptr};
    };
}
