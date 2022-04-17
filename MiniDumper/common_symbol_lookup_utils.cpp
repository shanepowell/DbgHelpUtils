#include "common_symbol_lookup_utils.h"

#include <optional>

#include "DbgHelpUtils/mini_dump_memory_walker.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_utils.h"
#include "DbgHelpUtils/symbol_type_info.h"
#include "DbgHelpUtils/symbol_type_utils.h"

using namespace dlg_help_utils;

std::optional<dbg_help::symbol_type_info> get_type_info(std::wostream& log, stream_stack_dump::mini_dump_memory_walker const& walker, std::wstring const& symbol_type_name)
{
    auto type_symbol_info = walker.get_type_info(symbol_type_name);
    if(!type_symbol_info.has_value())
    {
        log << std::format(L"Failed to find symbol type: {0}\n", symbol_type_name);
    }

    return type_symbol_info;
}

std::optional<stream_utils::symbol_type_and_value> find_field_pointer_and_type(std::wostream& log, stream_stack_dump::mini_dump_memory_walker const& walker, dbg_help::symbol_type_info const& type_symbol_info, std::wstring const& symbol_type_name, uint64_t const address, std::wstring_view const field_name)
{
    auto address_value = find_field_pointer_type_and_value_in_type(walker, stream_utils::get_field_type_and_offset_in_type(type_symbol_info, symbol_type_name, field_name, dbg_help::sym_tag_enum::PointerType), address);
    if(!address_value.has_value())
    {
        log << std::format(L"Failed to find field {0} in {1}\n", field_name, symbol_type_name);
        return std::nullopt;
    }

    if(address_value.value().value == 0)
    {
        log << std::format(L"Field {0} in {1} is null\n", field_name, symbol_type_name);
        return std::nullopt;
    }

    return address_value;
}

std::optional<uint64_t> find_field_pointer(std::wostream& log, stream_stack_dump::mini_dump_memory_walker const& walker, dbg_help::symbol_type_info const& type_symbol_info, std::wstring const& symbol_type_name, uint64_t const address, std::wstring_view const field_name)
{
    auto address_value = find_field_pointer_and_type(log, walker, type_symbol_info, symbol_type_name, address, field_name);
    if(!address_value.has_value())
    {
        return std::nullopt;
    }

    return address_value.value().value;
}

std::optional<uint64_t> find_field_pointer(std::wostream& log, stream_stack_dump::mini_dump_memory_walker const& walker, std::wstring const& symbol_type_name, uint64_t const address, std::wstring_view const field_name)
{
    auto const type_symbol_info = get_type_info(log, walker, symbol_type_name);
    if(!type_symbol_info.has_value())
    {
        return std::nullopt;
    }

    return find_field_pointer(log, walker, type_symbol_info.value(), symbol_type_name, address, field_name);
}

std::optional<dbg_help::symbol_type_info> dump_field(std::wostream& log, stream_stack_dump::mini_dump_memory_walker const& walker, std::wstring const& symbol_type_name, uint64_t const address)
{
    auto type_symbol_info = get_type_info(log, walker, symbol_type_name);
    if(!type_symbol_info.has_value())
    {
        return std::nullopt;
    }

    auto const data_length = type_symbol_info.value().length();
    if(!data_length.has_value() || data_length.value() == 0)
    {
        log << std::format(L"Symbol {0} is zero length\n", symbol_type_name);
        return std::nullopt;
    }

    auto stream = walker.get_process_memory_stream(address, data_length.value());
    if(stream.eof())
    {
        log << std::format(L"Failed to find {0} address [{1}] in dump file\n", symbol_type_name, stream_hex_dump::to_hex_full(address));
        return std::nullopt;
    }
    symbol_type_utils::dump_variable_symbol_at(log, walker, type_symbol_info.value(), type_symbol_info.value(), address, stream);
    return type_symbol_info;
}
