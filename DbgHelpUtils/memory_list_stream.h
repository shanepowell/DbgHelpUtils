#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <vector>
#include "generator.h"
#include <limits>

namespace dlg_help_utils
{
    struct memory_range;
    class mini_dump;

    class memory_list_stream
    {
    public:
        memory_list_stream() = default;
        explicit memory_list_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_MEMORY_LIST const& memory_list() const { return *memory_list_; }

        [[nodiscard]] std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*> const& list() const { return memory_address_ranges_; }

        [[nodiscard]] void const* find_address_range(uint64_t address, uint64_t length) const;
        [[nodiscard]] void const* find_any_address_range(uint64_t address, uint64_t& length) const;
        [[nodiscard]] dlg_help_utils::generator<memory_range> memory_ranges() const;

    private:
        [[nodiscard]] std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*>::const_iterator memory_address_ranges_upper_bound(uint64_t address) const;
        [[nodiscard]] static MINIDUMP_MEMORY_LIST const* get_memory_list(mini_dump const& dump, size_t& index);
        [[nodiscard]] std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*> build_memory_address_ranges() const;

    private:
        mini_dump const* dump_{nullptr};
        MINIDUMP_MEMORY_LIST const* memory_list_{nullptr};
        size_t index_{};
        std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*> memory_address_ranges_{build_memory_address_ranges()};
        bool found_{false};
    };
}
