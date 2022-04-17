#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <vector>
#include <experimental/generator>

namespace dlg_help_utils
{
    struct memory_range;
    class mini_dump;

    class memory_list_stream
    {
    public:
        explicit memory_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_MEMORY_LIST const& memory_list() const { return *memory_list_; }

        [[nodiscard]] std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*> const& list() const { return memory_address_ranges_; }

        [[nodiscard]] void const* find_address_range(uint64_t address, uint64_t length) const;
        [[nodiscard]] void const* find_any_address_range(uint64_t address, uint64_t& length) const;
        [[nodiscard]] std::experimental::generator<memory_range> memory_ranges() const;

    private:
        [[nodiscard]] std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*>::const_iterator memory_address_ranges_upper_bound(uint64_t address) const;
        [[nodiscard]] static MINIDUMP_MEMORY_LIST const* get_memory_list(mini_dump const& dump, size_t& index);
        [[nodiscard]] std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*> build_memory_address_ranges() const;

    private:
        mini_dump const& dump_;
        MINIDUMP_MEMORY_LIST const* memory_list_;
        size_t const index_;
        std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*> memory_address_ranges_{build_memory_address_ranges()};
        bool const found_;
    };
}
