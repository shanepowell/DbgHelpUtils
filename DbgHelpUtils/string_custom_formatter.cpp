#include "string_custom_formatter.h"

#include <format>

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "i_value_type_formatter.h"
#include "mini_dump_memory_stream.h"
#include "mini_dump_memory_walker.h"
#include "module_list_stream.h"
#include "symbol_data_dumper.h"
#include "symbol_type_utils.h"

namespace dlg_help_utils::ntdll_utilities
{
    std::wstring const& string_custom_formatter::symbol_name = common_symbol_names::string_symbol_name;

    string_custom_formatter::string_custom_formatter(cache_manager& cache)
    : cache_manager_{&cache}
    {
    }

    bool string_custom_formatter::is_custom_type(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , dbg_help::symbol_type_info const& type
        , [[maybe_unused]] std::wstring_view const& path
        , [[maybe_unused]] std::wstring_view const& name
        , [[maybe_unused]] std::vector<dbg_help::symbol_type_info> const& parents)
    {
        return is_type_equal(walker, type, path, name, parents, get_cache_data(walker).string_symbol_type);
    }

    symbol_type_utils::dump_variable_symbol_data_result string_custom_formatter::format(
        std::function<std::wstring()> original_render_line
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , [[maybe_unused]] symbol_type_utils::symbol_visit_flags::flags const options
        , dbg_help::symbol_type_info const& type
        , [[maybe_unused]] dbg_help::sym_tag_enum const tag
        , uint64_t variable_address
        , mini_dump_memory_stream& variable_stream
        , [[maybe_unused]] std::wstring_view const& path
        , [[maybe_unused]] std::wstring_view const& name
        , [[maybe_unused]] std::unordered_set<uint64_t>& visited_pointers
        , [[maybe_unused]] size_t const max_symbol_dump_depth
        , [[maybe_unused]] std::vector<dbg_help::symbol_type_info> const& parents
        , symbol_type_utils::symbol_data_dumper const& dumper)
    {
        variable_address = get_address(walker, type, variable_address, variable_stream, dumper.formatter());

        auto const length = stream_utils::find_basic_type_field_value_in_type<uint16_t>(walker, cache_data_->length_field_data, variable_address);
        auto const maximum_length = stream_utils::find_basic_type_field_value_in_type<uint16_t>(walker, cache_data_->maximum_length_field_data, variable_address);
        auto const address_value = stream_utils::find_field_pointer_type_and_value_in_type(walker, cache_data_->buffer_field_data, variable_address);

        if (!length.has_value() || !maximum_length.has_value() || !address_value.has_value())
        {
            return {};
        }

        auto string_stream = walker.get_process_memory_stream(address_value.value().value, length.value());
        if (string_stream.eof())
        {
            if (address_value.value().value == 0)
            {
                return
                {
                    .result = symbol_type_utils::symbol_type_custom_formatter_result::stop,
                    .render_line = [original_render_line = std::move(original_render_line), length = length.value(), maximum_length = maximum_length.value(), address_value = address_value.value().value, &formatter = dumper.formatter()]
                    {
                        // ReSharper disable once StringLiteralTypo
                        return std::format(L"{}: Addr:{}:MaxLen:{}:Len:{}", original_render_line(), formatter.format_pointer_value(address_value, true), formatter.format_value(maximum_length), formatter.format_value(length));
                    }
                };
            }

            return
            {
                .result = symbol_type_utils::symbol_type_custom_formatter_result::stop,
                .render_line = [original_render_line = std::move(original_render_line), length = length.value(), maximum_length = maximum_length.value(), address_value = address_value.value().value, &formatter = dumper.formatter()]
                {
                    // ReSharper disable once StringLiteralTypo
                    return std::format(L"{}: Addr:{}:MaxLen:{}:Len:{} - {}", original_render_line(), formatter.format_pointer_value(address_value, true), formatter.format_value(maximum_length), formatter.format_value(length), symbol_type_utils::resources::get_variable_unknown());
                }
            };
        }

        auto string = mini_dump_string_stream<char>{string_stream, length.value() / sizeof(char), stop_at_null_t{false}};

        return
        {
            .result = symbol_type_utils::symbol_type_custom_formatter_result::stop,
            .render_line = [original_render_line = std::move(original_render_line), length = length.value(), maximum_length = maximum_length.value(), address_value = address_value.value().value, string = std::move(string), &formatter = dumper.formatter()]
            {
                // ReSharper disable once StringLiteralTypo
                return std::format(L"{}: Addr:{}:MaxLen:{}:Len:{}:[{}]", original_render_line(), formatter.format_pointer_value(address_value, true), formatter.format_value(maximum_length), formatter.format_value(length), formatter.format_value(string));
            }
        };
    }

    string_custom_formatter::cache_data const& string_custom_formatter::get_cache_data(stream_stack_dump::mini_dump_memory_walker const& walker) const
    {
        if (!cache_data_)
        {
            cache_data_ = &setup_globals(walker);
        }
        return *cache_data_;
    }

    string_custom_formatter::cache_data const& string_custom_formatter::setup_globals(stream_stack_dump::mini_dump_memory_walker const& walker) const
    {
        if(!cache().has_cache<cache_data>())
        {
            auto& data = cache().get_cache<cache_data>();
            data.string_symbol_type = stream_utils::get_type(walker, symbol_name);
            data.length_field_data = stream_utils::get_field_type_and_offset_in_type(data.string_symbol_type, symbol_name, common_symbol_names::string_length_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.maximum_length_field_data = stream_utils::get_field_type_and_offset_in_type(data.string_symbol_type, symbol_name, common_symbol_names::string_maximum_length_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.buffer_field_data = stream_utils::get_field_type_and_offset_in_type(data.string_symbol_type, symbol_name, common_symbol_names::string_buffer_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
        }

        return cache().get_cache<cache_data>();
    }
}
