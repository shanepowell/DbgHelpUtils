#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include "mini_dump_memory_stream.h"
#include "symbol_type_custom_formatter.h"
#include "symbol_visit_flags.h"
#include "tagged_bool.h"

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
    using is_head_t = tagged_bool<struct is_head_type>;

    class symbol_data_dumper
    {
    public:
        symbol_data_dumper(i_value_type_formatter const& formatter)
            : formatter_(&formatter)
        {
        }

        [[nodiscard]] dump_variable_symbol_data variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , std::wstring_view const& prefix
            , dbg_help::symbol_type_info const& type
            , dbg_help::symbol_type_info const& display_type
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , is_head_t is_head
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth = 0
            , std::vector<dbg_help::symbol_type_info> const& parents = {}
            ) const;

        void register_custom_type_formatter(std::unique_ptr<symbol_type_custom_formatter> formatter);

        i_value_type_formatter const& formatter() const { return *formatter_; }


        static bool can_dump_tag(dbg_help::sym_tag_enum tag);
        static void fix_wow64_pointer(uint64_t& value, stream_stack_dump::mini_dump_memory_walker const& walker, i_value_type_formatter const& formatter);

    private:
        struct pointer_data_t
        {
            uint64_t pointer_value;
            dbg_help::symbol_type_info pointer_type;
            mini_dump_memory_stream variable_stream;
        };

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_data_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::optional<dbg_help::symbol_type_info> const& data_type
            , std::optional<dbg_help::sym_tag_enum> const& data_type_tag
            , unsigned long long bit_mask
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_pointer_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_base_type_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t bit_mask
            , is_pointer_t is_pointer
            , size_t max_size
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_enum_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_array_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_unsupported_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> dump_pointer_type_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , unsigned long long bit_mask
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents) const;

        void do_dump_pointer_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , mini_dump_memory_stream& variable_stream) const;

        void do_dump_base_type_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t bit_mask
            , is_pointer_t is_pointer
            , size_t max_size) const;

        void do_dump_enum_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , mini_dump_memory_stream variable_stream) const;

        void do_dump_array_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream) const;

        static void do_dump_unsupported_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag);

        template<typename T>
        void dump_char_variable(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , is_pointer_t is_pointer
            , size_t max_size) const;

        template<typename T>
        void dump_number_variable(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , mini_dump_memory_stream& variable_stream
            , uint64_t bit_mask) const;

        void dump_pointer_memory_value(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , pointer_data_t const& pointer_data) const;

        [[nodiscard]] std::optional<dump_variable_symbol_data> process_dump_value(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum ta
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents
            , std::function<std::wstring()> original_value) const;

        generator<dump_variable_symbol_data> data_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info type
            , uint64_t variable_address
            , mini_dump_memory_stream variable_stream
            , std::optional<dbg_help::symbol_type_info> data_type
            , std::optional<dbg_help::sym_tag_enum> data_type_tag
            , std::wstring path
            , std::wstring name
            , std::unordered_set<uint64_t> visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> parents) const;

        generator<dump_variable_symbol_data> array_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info type
            , uint64_t variable_address
            , [[maybe_unused]] mini_dump_memory_stream variable_stream
            , std::wstring path
            , std::wstring name
            , std::unordered_set<uint64_t> visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> parents) const;

        generator<dump_variable_symbol_data> pointer_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info type
            , [[maybe_unused]] uint64_t variable_address
            , mini_dump_memory_stream variable_stream
            , std::wstring path
            , std::wstring name
            , std::unordered_set<uint64_t> visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> parents) const;

        generator<dump_variable_symbol_data> variable_symbol_udt_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info type
            , [[maybe_unused]] dbg_help::symbol_type_info display_type
            , uint64_t variable_address
            , [[maybe_unused]] mini_dump_memory_stream variable_stream
            , std::wstring path
            , std::wstring name
            , std::unordered_set<uint64_t> visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> parents) const;

        generator<dump_variable_symbol_data> children_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info type
            , is_head_t is_head
            , mini_dump_memory_stream variable_stream
            , uint64_t variable_address
            , std::wstring path
            , std::unordered_set<uint64_t> visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> parents) const;

        generator<dump_variable_symbol_data> base_type_variable_symbol_at(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream const& variable_stream
            , is_pointer_t is_pointer
            , size_t max_size
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
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
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> parents) const;

        generator<dump_variable_symbol_data> dump_udt_array(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream variable_stream
            , size_t max_size
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> parents) const;

        generator<dump_variable_symbol_data> pointer_memory_value(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , uint64_t pointer_value
            , dbg_help::symbol_type_info const& pointer_type
            , mini_dump_memory_stream const& variable_stream
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info>& parents) const;

        bool any_data_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::optional<dbg_help::symbol_type_info> const& data_type
            , std::optional<dbg_help::sym_tag_enum> data_type_tag
            , std::unordered_set<uint64_t>& visited_pointers
            ) const;

        std::optional<pointer_data_t> get_pointer(
            stream_stack_dump::mini_dump_memory_walker const& walker
                , dbg_help::symbol_type_info const& type
                , mini_dump_memory_stream& variable_stream) const;

        static bool any_udt_array(dbg_help::symbol_type_info const& type, size_t max_size);

        static bool any_variable_symbol_at(dbg_help::symbol_type_info const& type);

        bool any_pointer_memory_value(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& pointer_type
            , uint64_t pointer_value
            , mini_dump_memory_stream& variable_stream) const;

        bool any_pointer_variable_symbol_at(
            std::function<std::wstring()>& render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , mini_dump_memory_stream& variable_stream
            , std::unordered_set<uint64_t>& visited_pointers
            ) const;

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

        static void dump_bitmask(std::function<std::wstring()>& render_line, dbg_help::symbol_type_info const& type, unsigned long long& bit_mask);
        static bool detect_pointer_cycle(
            std::function<std::wstring()>& render_line
            , symbol_visit_flags::flags options
            , uint64_t pointer
            , std::unordered_set<uint64_t>& visited_pointers
            );

        [[nodiscard]] static std::wstring_view remove_leaf(std::wstring_view const& path);

    private:
        std::vector<std::unique_ptr<symbol_type_custom_formatter>> custom_formatters_;
        i_value_type_formatter const* formatter_;
    };
}
