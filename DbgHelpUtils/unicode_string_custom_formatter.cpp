#include "unicode_string_custom_formatter.h"

#include <format>

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "i_value_type_formatter.h"
#include "mini_dump_memory_stream.h"
#include "mini_dump_memory_walker.h"
#include "module_list_stream.h"
#include "stream_hex_dump.h"

namespace dlg_help_utils::ntdll_utilities
{
    std::wstring const& unicode_string_custom_formatter::symbol_name = common_symbol_names::unicode_string_symbol_name;

    unicode_string_custom_formatter::unicode_string_custom_formatter(cache_manager& cache)
    : cache_manager_{&cache}
    {
    }

    bool unicode_string_custom_formatter::is_custom_type(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , dbg_help::symbol_type_info const& type
        , [[maybe_unused]] std::wstring_view const& path
        , [[maybe_unused]] std::wstring_view const& name
        , [[maybe_unused]] std::vector<dbg_help::symbol_type_info> const& parents)
    {
        return is_type_equal(walker, type, path, name, parents, get_cache_data(walker).unicode_string_symbol_type);
    }

    symbol_type_utils::dump_variable_symbol_data_result unicode_string_custom_formatter::format(
        std::function<std::wstring()> original_render_line
        , [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker
        , [[maybe_unused]] dbg_help::symbol_type_info const& type
        , [[maybe_unused]] dbg_help::sym_tag_enum const tag
        , uint64_t const variable_address
        , [[maybe_unused]] mini_dump_memory_stream& variable_stream
        , [[maybe_unused]] std::wstring_view const& path
        , [[maybe_unused]] std::wstring_view const& name
        , [[maybe_unused]] std::vector<dbg_help::symbol_type_info> const& parents
        , [[maybe_unused]] i_value_type_formatter const& formatter)
    {
        auto const length = stream_utils::find_basic_type_field_value_in_type<uint16_t>(walker, cache_data_->length_field_data, variable_address);
        auto const maximum_length = stream_utils::find_basic_type_field_value_in_type<uint16_t>(walker, cache_data_->maximum_length_field_data, variable_address);
        auto const address_value = stream_utils::find_field_pointer_type_and_value_in_type(walker, cache_data_->buffer_field_data, variable_address);

        if (!length.has_value() || !maximum_length.has_value() || !address_value.has_value())
        {
            return {};
        }

        auto string_stream = walker.get_process_memory_stream(address_value.value().value, length.value());
        auto string = mini_dump_string_stream<wchar_t>{string_stream, length.value() / sizeof(wchar_t), stop_at_null_t{false}};

        return
        {
            .result = symbol_type_utils::symbol_type_custom_formatter_result::stop,
            .render_line = [original_render_line = std::move(original_render_line), length = length.value(), maximum_length = maximum_length.value(), address_value = address_value.value().value, string = std::move(string), &formatter]
            {
                // ReSharper disable once StringLiteralTypo
                return std::format(L"{}: Addr:{}:MaxLen:{}:Len:{}:[{}]", original_render_line(), formatter.format_pointer_value(address_value), formatter.format_value(maximum_length), formatter.format_value(length), formatter.format_value(string));
            }
        };
    }

    unicode_string_custom_formatter::cache_data const& unicode_string_custom_formatter::get_cache_data(stream_stack_dump::mini_dump_memory_walker const& walker) const
    {
        if (!cache_data_)
        {
            cache_data_ = &setup_globals(walker);
        }
        return *cache_data_;
    }

    unicode_string_custom_formatter::cache_data const& unicode_string_custom_formatter::setup_globals(stream_stack_dump::mini_dump_memory_walker const& walker) const
    {
        if(!cache().has_cache<cache_data>())
        {
            auto& data = cache().get_cache<cache_data>();
            data.unicode_string_symbol_type = stream_utils::get_type(walker, symbol_name);
            data.length_field_data = stream_utils::get_field_type_and_offset_in_type(data.unicode_string_symbol_type, symbol_name, common_symbol_names::unicode_string_length_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.maximum_length_field_data = stream_utils::get_field_type_and_offset_in_type(data.unicode_string_symbol_type, symbol_name, common_symbol_names::unicode_string_maximum_length_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.buffer_field_data = stream_utils::get_field_type_and_offset_in_type(data.unicode_string_symbol_type, symbol_name, common_symbol_names::unicode_string_buffer_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
        }

        return cache().get_cache<cache_data>();
    }
}
