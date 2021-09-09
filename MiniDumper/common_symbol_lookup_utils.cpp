﻿#include "common_symbol_lookup_utils.h"

#include <iostream>
#include <optional>

#include "DbgHelpUtils/mini_dump_stack_walk.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_utils.h"
#include "DbgHelpUtils/symbol_type_info.h"
#include "DbgHelpUtils/symbol_type_utils.h"

using namespace dlg_help_utils;

std::optional<dbg_help::symbol_type_info> get_type_info(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_type_name)
{
    auto const type_symbol_info = walker.get_type_info(symbol_type_name);
    if(!type_symbol_info.has_value())
    {
        std::wcout << "Failed to find symbol type: " << symbol_type_name << "\n";
    }

    return type_symbol_info;
}

std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> find_field_pointer_and_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type_symbol_info, std::wstring const& symbol_type_name, uint64_t const address, std::wstring_view const field_name)
{
    auto address_value = stream_utils::find_field_pointer_type_and_value_in_type(walker, type_symbol_info, field_name, address);
    if(!address_value.has_value())
    {
        std::wcout << "Failed to find field " << field_name << " in " << symbol_type_name << '\n';
        return std::nullopt;
    }

    if(address_value.value().second == 0)
    {
        std::wcout << "Field " << field_name << " in " << symbol_type_name << " is null\n";
        return std::nullopt;
    }

    return address_value.value();
}

std::optional<uint64_t> find_field_pointer(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type_symbol_info, std::wstring const& symbol_type_name, uint64_t const address, std::wstring_view const field_name)
{
    auto address_value = find_field_pointer_and_type(walker, type_symbol_info, symbol_type_name, address, field_name);
    if(!address_value.has_value())
    {
        return std::nullopt;
    }

    return address_value.value().second;
}

std::optional<uint64_t> find_field_pointer(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_type_name, uint64_t const address, std::wstring_view const field_name)
{
    auto const type_symbol_info = get_type_info(walker, symbol_type_name);
    if(!type_symbol_info.has_value())
    {
        return std::nullopt;
    }

    return find_field_pointer(walker, type_symbol_info.value(), symbol_type_name, address, field_name);
}

std::optional<dbg_help::symbol_type_info> dump_field(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_type_name, uint64_t const address)
{
    auto const type_symbol_info = get_type_info(walker, symbol_type_name);
    if(!type_symbol_info.has_value())
    {
        return std::nullopt;
    }

    auto const data_length = type_symbol_info.value().length();
    if(!data_length.has_value() || data_length.value() == 0)
    {
        std::wcout << "Symbol " << symbol_type_name << " is zero length\n";
        return std::nullopt;
    }

    auto stream = walker.get_process_memory_stream(address, data_length.value());
    if(stream.eof())
    {
        std::wcout << "Failed to find " << symbol_type_name << " address [" << stream_hex_dump::to_hex_full(address) << "] in dump file\n";
        return std::nullopt;
    }
    symbol_type_utils::dump_variable_symbol_at(std::wcout, walker, type_symbol_info.value(), type_symbol_info.value(), address, stream);
    return type_symbol_info;
}