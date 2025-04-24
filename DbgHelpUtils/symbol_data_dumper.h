#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include "dump_hex.h"
#include "symbol_type_custom_formatter.h"
#include "tagged_bool.h"

namespace dlg_help_utils
{
    class mini_dump_memory_stream;
}

namespace dlg_help_utils::dbg_help
{
    class symbol_type_info;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::symbol_type_utils
{
    constexpr static inline auto g_all_bits = std::numeric_limits<uint64_t>::max();
    using is_pointer_t = tagged_bool<struct is_pointer_type>;

    namespace symbol_visit_flags
    {
        enum flags : uint8_t
        {
            none = 0x0,
            detect_pointer_cycles = 0x1,
            no_header = 0x2
        };
    };


    class symbol_data_dumper
    {
    public:
        [[nodiscard]] dump_variable_symbol_data variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , std::wstring_view const& prefix
            , dbg_help::symbol_type_info const& type
            , dbg_help::symbol_type_info const& display_type
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> const& parents = {}
            ) const;

        void register_custom_type_formatter(std::unique_ptr<symbol_type_custom_formatter> formatter);

        static bool can_dump_tag(dbg_help::sym_tag_enum tag);

    private:
        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_data_at(
            std::wstringstream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::optional<dbg_help::symbol_type_info> const& data_type
            , std::optional<dbg_help::sym_tag_enum> const& data_type_tag
            , unsigned long long bit_mask
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_pointer_variable_symbol_at(
            std::wstringstream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_base_type_variable_symbol_at(
            std::wstringstream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t bit_mask
            , is_pointer_t is_pointer
            , size_t max_size
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_enum_variable_symbol_at(
            std::wstringstream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_array_variable_symbol_at(
            std::wstringstream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_unsupported_variable_symbol_at(
            std::wstringstream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_pointer_type_at(
            std::wstringstream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , unsigned long long bit_mask
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        void do_dump_pointer_variable_symbol_at(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , mini_dump_memory_stream& variable_stream) const;

        void do_dump_base_type_variable_symbol_at(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t bit_mask
            , is_pointer_t is_pointer
            , size_t max_size) const;

        void do_dump_enum_variable_symbol_at(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream variable_stream) const;

        void do_dump_array_variable_symbol_at(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream) const;

        static void do_dump_unsupported_variable_symbol_at(
            std::wostream& os
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag);

        template<typename T>
        static void dump_string(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t max_size
            , uint64_t limit_size = 256);

        template<typename T>
        void dump_char_variable(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , is_pointer_t is_pointer
            , size_t max_size) const;

        template<typename T>
        void dump_number_variable(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t bit_mask
            , is_pointer_t is_pointer
            , size_t max_size
            , dump_hex_t dump_hex) const;

        template<typename T>
        void dump_pointer_memory_value(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , T const& pointer_value) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> process_dump_value(
            std::wstringstream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum ta
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents
            , std::wstring_view const& original_value) const;

        generator<dump_variable_symbol_data> data_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::optional<dbg_help::symbol_type_info> const& data_type
            , std::optional<dbg_help::sym_tag_enum> data_type_tag
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        generator<dump_variable_symbol_data> array_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , [[maybe_unused]] mini_dump_memory_stream const& variable_stream
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> parents) const;

        generator<dump_variable_symbol_data> pointer_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , [[maybe_unused]] uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        generator<dump_variable_symbol_data> variable_symbol_udt_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , [[maybe_unused]] dbg_help::symbol_type_info const& display_type
            , uint64_t variable_address
            , [[maybe_unused]] mini_dump_memory_stream& variable_stream
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        generator<dump_variable_symbol_data> children_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , mini_dump_memory_stream const& variable_stream
            , uint64_t variable_address
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        generator<dump_variable_symbol_data> base_type_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , is_pointer_t is_pointer
            , size_t max_size
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        template<typename T>
        generator<dump_variable_symbol_data> variable_pointer_array(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream variable_stream
            , is_pointer_t is_pointer
            , std::optional<dbg_help::symbol_type_info> const& data_type
            , std::optional<dbg_help::sym_tag_enum> data_type_tag
            , size_t max_size
            , dump_hex_t dump_hex
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> parents) const;

        generator<dump_variable_symbol_data> dump_udt_array(
            stream_stack_dump::mini_dump_memory_walker const&walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream variable_stream
            , size_t max_size
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> parents) const;

        template<typename T>
        generator<dump_variable_symbol_data> pointer_memory_value(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , T const& pointer_value
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , std::vector<dbg_help::symbol_type_info> parents) const;

        static bool any_data_at(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::optional<dbg_help::symbol_type_info> const& data_type
            , std::optional<dbg_help::sym_tag_enum> data_type_tag
            , std::unordered_set<uint64_t>& visited_pointers
            );

        static bool any_udt_array(dbg_help::symbol_type_info const& type, size_t max_size);

        static bool any_variable_symbol_at(dbg_help::symbol_type_info const& type);

        template<typename T>
        static bool any_pointer_memory_value(stream_stack_dump::mini_dump_memory_walker const& walker, dbg_help::symbol_type_info const& type, T const& pointer_value);

        static bool any_pointer_variable_symbol_at(
            std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , mini_dump_memory_stream& variable_stream
            , std::unordered_set<uint64_t>& visited_pointers
            );

        static bool any_array_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address);

        static bool any_base_type_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , is_pointer_t is_pointer
            , size_t max_size);

        static bool any_number_variable(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t variable_address
            , is_pointer_t is_pointer
            , size_t element_size
            , size_t max_size);

        static void dump_bitmask(std::wostream& os, dbg_help::symbol_type_info const& type, unsigned long long& bit_mask);
        static bool detect_pointer_cycle(
            std::wostream& os
            , symbol_visit_flags::flags options
            , uint64_t pointer
            , std::unordered_set<uint64_t>& visited_pointers
            );
        static void fix_system_pointer(uint64_t& value);

    private:
        std::vector<std::unique_ptr<symbol_type_custom_formatter>> custom_formatters_;
    };
}
