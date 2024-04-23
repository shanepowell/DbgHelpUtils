#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <vector>
#include <experimental/generator>
#include <limits>

namespace dlg_help_utils
{
    struct memory_range;
    class mini_dump;

    struct memory64_entry
    {
        ULONG64 start_of_memory_range;
        ULONG64 end_of_memory_range;
        MINIDUMP_LOCATION_DESCRIPTOR64 location;
    };

    class memory64_list_stream
    {
    public:
        explicit memory64_list_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_MEMORY64_LIST const& memory_list() const { return *memory_list_; }

        [[nodiscard]] std::vector<memory64_entry> const& list() const { return memory_address_ranges_; }

        [[nodiscard]] void const* find_address_range(uint64_t address, uint64_t length) const;
        [[nodiscard]] void const* find_any_address_range(uint64_t address, uint64_t& length) const;
        [[nodiscard]] std::experimental::generator<memory_range> memory_ranges() const;

    private:
        [[nodiscard]] std::vector<memory64_entry>::const_iterator memory_address_ranges_upper_bound(uint64_t address) const;
        [[nodiscard]] static MINIDUMP_MEMORY64_LIST const* get_memory_list(mini_dump const& dump, size_t& index);
        [[nodiscard]] std::vector<memory64_entry> build_memory_address_ranges() const;

    private:
        mini_dump const& dump_;
        MINIDUMP_MEMORY64_LIST const* memory_list_;
        size_t const index_;
        std::vector<memory64_entry> const memory_address_ranges_{build_memory_address_ranges()};
        bool const found_;
    };
}
