#include "stream_utils.h"

#include <format>

#include "mini_dump.h"
#include "process_environment_block.h"
#include "stream_hex_dump.h"
#include "symbol_type_info.h"
#include "sym_tag_enum.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::stream_utils
{
    std::optional<std::tuple<size_t, MINIDUMP_DIRECTORY const*>> find_stream_for_type(mini_dump const& dump_file, MINIDUMP_STREAM_TYPE const type)
    {
        auto const* header = dump_file.header();
        if (header == nullptr)
        {
            return std::nullopt;
        }

        auto const* directory = dump_file.directory();
        if (directory == nullptr)
        {
            return std::nullopt;
        }

        for (size_t index = 0; index < header->NumberOfStreams; ++index)
        {
            if (auto const & entry = directory[index]; static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType) == type)
            {
                return std::make_tuple(index, &entry);
            }
        }

        return std::nullopt;
    }

    std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> find_field_pointer_type_and_value(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& pointer_type, uint64_t memory_address, size_t const index)
    {
        if(auto const type_tag_data = pointer_type.sym_tag(); type_tag_data.value_or(dbg_help::sym_tag_enum::Null) != dbg_help::sym_tag_enum::PointerType)
        {
            return std::nullopt;
        }

        auto const data_type_length = pointer_type.length();
        if(!data_type_length.has_value())
        {
            return std::nullopt;
        }

        auto const data_type = pointer_type.type();
        if(!data_type.has_value())
        {
            return std::nullopt;
        }

        memory_address += (index * data_type_length.value());

        std::optional<uint64_t> value;
        switch(data_type_length.value())
        {
        case 4:
            value = read_field_value<uint32_t, uint64_t>(walker, memory_address);
            break;

        case 8:
            value = read_field_value<uint64_t, uint64_t>(walker, memory_address);
            break;

        default:
            return std::nullopt;
        }

        if(!value.has_value())
        {
            return std::nullopt;
        }

        return std::make_pair(data_type.value(), value.value());
    }

    std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> find_field_pointer_array_type_and_value(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& array_type, uint64_t memory_address, size_t const index)
    {
        if(auto const type_tag_data = array_type.sym_tag(); type_tag_data.value_or(dbg_help::sym_tag_enum::Null) != dbg_help::sym_tag_enum::ArrayType)
        {
            return std::nullopt;
        }

        auto const pointer_type = array_type.type();
        if(!pointer_type.has_value())
        {
            return std::nullopt;
        }

        if(auto const type_tag_data = pointer_type.value().sym_tag(); type_tag_data.value_or(dbg_help::sym_tag_enum::Null) != dbg_help::sym_tag_enum::PointerType)
        {
            return std::nullopt;
        }


        auto const array_count = array_type.array_count();
        if(!array_count.has_value())
        {
            return std::nullopt;
        }

        auto const data_type_length = pointer_type.value().length();
        if(!data_type_length.has_value())
        {
            return std::nullopt;
        }

        auto const data_type = pointer_type.value().type();
        if(!data_type.has_value())
        {
            return std::nullopt;
        }

        if(index < 0 || index > array_count.value())
        {
            return std::nullopt;
        }

        memory_address += (index * data_type_length.value());

        std::optional<uint64_t> value;
        switch(data_type_length.value())
        {
        case 4:
            value = read_field_value<uint32_t, uint64_t>(walker, memory_address);
            break;

        case 8:
            value = read_field_value<uint64_t, uint64_t>(walker, memory_address);
            break;

        default:
            return std::nullopt;
        }

        if(!value.has_value())
        {
            return std::nullopt;
        }

        return std::make_pair(data_type.value(), value.value());
    }

    std::optional<symbol_type_and_base_type_field_offset> find_field_type_and_offset_in_type(dbg_help::symbol_type_info const& type, std::wstring_view const field_name, dbg_help::sym_tag_enum const tag)
    {
        auto const field_data = type.find_field_in_type(field_name);
        if(!field_data.has_value())
        {
            return std::nullopt;
        }

        auto const [field, base_offset] = field_data.value();

        auto const data_type = field.type(); 
        if(!data_type.has_value())
        {
            return std::nullopt;
        }

        if(auto const data_type_tag = data_type.value().sym_tag(); data_type_tag.value_or(dbg_help::sym_tag_enum::Null) != tag)
        {
            return std::nullopt;
        }

        auto const offset_data = field.offset();
        if(!offset_data.has_value())
        {
            return std::nullopt;
        }

        return symbol_type_and_base_type_field_offset{data_type.value(), offset_data.value() + base_offset};
    }

    symbol_type_and_base_type_field_offset get_field_type_and_offset_in_type(dbg_help::symbol_type_info const& type, std::wstring_view const type_name, std::wstring_view const field_name, dbg_help::sym_tag_enum const tag)
    {
        auto rv = find_field_type_and_offset_in_type(type, field_name, tag);
        if(!rv.has_value())
        {
            throw_cant_get_field_data(type_name, field_name);
        }

        return rv.value();
    }

    std::optional<symbol_type_and_value> find_field_pointer_type_and_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, symbol_type_and_base_type_field_offset const& field_data, uint64_t const memory_address)
    {
        auto const data_type_length = field_data.type.length();
        if(!data_type_length.has_value())
        {
            return std::nullopt;
        }

        std::optional<uint64_t> value;
        switch(data_type_length.value())
        {
        case 4:
            value = read_field_value<uint32_t, uint64_t>(walker, memory_address + field_data.field_offset);
            break;

        case 8:
            value = read_field_value<uint64_t, uint64_t>(walker, memory_address + field_data.field_offset);
            break;

        default:
            return std::nullopt;
        }

        if(!value.has_value())
        {
            return std::nullopt;
        }

        return symbol_type_and_value{field_data.type, value.value()};
    }

    std::optional<uint64_t> find_field_pointer_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, symbol_type_and_base_type_field_offset const& field_data, uint64_t const memory_address)
    {
        auto const value = find_field_pointer_type_and_value_in_type(walker, field_data, memory_address);
        if(!value.has_value())
        {
            return std::nullopt;
        }

        return value.value().value;
    }

    std::optional<uint64_t> find_basic_type_field_address_in_type(symbol_type_and_base_type_field_offset const& field_data, uint64_t const memory_address, size_t const field_size)
    {
        auto const data_type_length = field_data.type.length();
        if(!data_type_length.has_value())
        {
            return std::nullopt;
        }

        if(data_type_length.value() == field_size)
        {
            return memory_address + field_data.field_offset;
        }

        return std::nullopt;
    }

    std::optional<std::tuple<uint64_t, uint32_t, uint32_t>> find_bit_type_field_address_in_type(dbg_help::symbol_type_and_field_offset const& field_data, uint64_t const memory_address, size_t const field_size)
    {
        auto const data_type = field_data.type.type(); 
        if(!data_type.has_value())
        {
            return std::nullopt;
        }

        if(auto const data_type_tag = data_type.value().sym_tag(); data_type_tag.value_or(dbg_help::sym_tag_enum::Null) != dbg_help::sym_tag_enum::BaseType)
        {
            return std::nullopt;
        }

        auto const offset_data = field_data.type.offset();
        if(!offset_data.has_value())
        {
            return std::nullopt;
        }

        auto const field_bit_length = field_data.type.length();
        if(!field_bit_length.has_value())
        {
            return std::nullopt;
        }

        auto const data_type_length = data_type.value().length();
        if(!data_type_length.has_value())
        {
            return std::nullopt;
        }

        if(data_type_length.value() <= field_size)
        {
            if(auto const bit_position_data = field_data.type.bit_position(); bit_position_data.has_value())
            {
                return std::make_tuple(memory_address + offset_data.value(), bit_position_data.value(), static_cast<uint32_t>(field_bit_length.value()));
            }
        }

        return std::nullopt;
    }

    std::optional<std::tuple<std::unique_ptr<uint8_t[]>, uint64_t, uint64_t>> read_udt_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, symbol_type_and_base_type_field_offset const& field_data, uint64_t const memory_address)
    {
        auto const data_type_length = field_data.type.length();
        if(!data_type_length.has_value())
        {
            return std::nullopt;
        }

        if(data_type_length.value() == 0)
        {
            return std::nullopt;
        }

        auto const length = static_cast<size_t>(data_type_length.value());
        auto buffer = std::make_unique<uint8_t[]>(length);
        auto stream = walker.get_process_memory_stream(memory_address + field_data.field_offset, data_type_length.value());

        if(stream.eof())
        {
            return std::nullopt;
        }

        if(stream.read(buffer.get(), length) != data_type_length.value())
        {
            return std::nullopt;
        }
        return std::make_tuple(std::move(buffer), data_type_length.value(), memory_address + field_data.field_offset);
    }

    uint32_t machine_field_size(process::process_environment_block const& peb)
    {
        if(peb.is_x86_target())
        {
            return sizeof(uint32_t);
        }

        if(peb.is_x64_target())
        {
            return sizeof(uint64_t);
        }

        return 0;
    }

    std::optional<uint64_t> read_machine_size_field_value(process::process_environment_block const& peb, uint64_t const memory_address)
    {
        if(peb.is_x86_target())
        {
            return read_field_value<uint32_t, uint64_t>(peb.walker(), memory_address);
        }

        if(peb.is_x64_target())
        {
            return read_field_value<uint64_t>(peb.walker(), memory_address);
        }

        return std::nullopt;
    }

    std::optional<uint64_t> find_machine_size_field_value(process::process_environment_block const& peb, symbol_type_and_base_type_field_offset const& field_data, uint64_t const memory_address)
    {
        if(peb.is_x86_target())
        {
            return find_basic_type_field_value_in_type<uint32_t>(peb.walker(), field_data, memory_address);
        }

        if(peb.is_x64_target())
        {
            return find_basic_type_field_value_in_type<uint64_t>(peb.walker(), field_data, memory_address);
        }

        return std::nullopt;
    }

    dbg_help::symbol_type_info get_type(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& name, bool const throw_on_error)
    {
        auto const symbol_info = walker.get_type_info(name, throw_on_error);
        if(!symbol_info.has_value())
        {
            throw exceptions::wide_runtime_error{std::format(L"Error: symbol {} not found", name)};
        }

        return symbol_info.value();
    }

    size_t get_type_length(dbg_help::symbol_type_info const& type, std::wstring const& type_name)
    {
        if(auto const length_data = type.length(); length_data.has_value())
        {
            return static_cast<size_t>(length_data.value());
        }

        throw exceptions::wide_runtime_error{std::format(L"Error: symbol {} length not found", type_name)};
    }

    uint64_t get_field_pointer_raw(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const address, symbol_type_and_base_type_field_offset const& field_data, std::wstring const& type_name, std::wstring const& field_name)
    {
        auto const address_value = find_field_pointer_type_and_value_in_type(walker, field_data, address);
        if(!address_value.has_value())
        {
            throw_cant_get_field_data(type_name, field_name);
        }

        return address_value.value().value;
    }

    uint64_t get_field_pointer(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const address, symbol_type_and_base_type_field_offset const& field_data, std::wstring const& type_name, std::wstring const& field_name)
    {
        auto const pointer_value = get_field_pointer_raw(walker, address, field_data, type_name, field_name);
        if(pointer_value == 0)
        {
            throw_cant_get_field_is_null(type_name, field_name);
        }

        return pointer_value;
    }

    uint64_t get_field_offset_from_type(dbg_help::symbol_type_info const& symbol_type, std::wstring const& symbol_name, std::wstring const& field_name)
    {
        if(auto const segment_entry = symbol_type.find_field_in_type(field_name); segment_entry.has_value())
        {
            if(auto const offset_data = segment_entry.value().type.offset(); offset_data.has_value())
            {
                return segment_entry.value().field_offset + offset_data.value();
            }
            throw exceptions::wide_runtime_error{std::format(L"Error: symbol {0} {1} field has no offset", symbol_name, field_name)};
        }
        throw exceptions::wide_runtime_error{std::format(L"Error: symbol {0} has no {1} field", symbol_name, field_name)};
    }

    uint64_t get_field_offset(dbg_help::symbol_type_and_field_offset const& field_data, std::wstring const& symbol_name, std::wstring const& field_name)
    {
        if(auto const offset_data = field_data.type.offset(); offset_data.has_value())
        {
            return field_data.field_offset + offset_data.value();
        }
        throw exceptions::wide_runtime_error{std::format(L"Error: symbol {0} {1} field has no offset", symbol_name, field_name)};
    }

    std::optional<uint64_t> read_static_variable_machine_size_value(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_name)
    {
        auto const symbol = walker.get_symbol_info(symbol_name);
        if(!symbol.has_value())
        {
            return std::nullopt;
        }

        auto const address = symbol.value().address();
        if(!address.has_value())
        {
            throw exceptions::wide_runtime_error{std::format(L"Error: symbol name {} can't read address", symbol_name)};
        }

        auto const length = symbol.value().length();
        if(!length.has_value())
        {
            throw exceptions::wide_runtime_error{std::format(L"Error: symbol name {} can't read length", symbol_name)};
        }

        switch(length.value())
        {
        case 4:
            if(auto const data = read_field_value<uint32_t>(walker, address.value()); data.has_value())
            {
                return data.value();
            }
            throw_failed_to_read_from_address(symbol_name, address.value());

        case 8:
            if(auto const data = read_field_value<uint64_t>(walker, address.value()); data.has_value())
            {
                return data.value();
            }
            throw_failed_to_read_from_address(symbol_name, address.value());

        default:
            throw exceptions::wide_runtime_error{std::format(L"Error: symbol name {0} can't length [{1}] not supported", symbol_name, length.value())};
        }
    }

    std::optional<std::pair<uint64_t, uint64_t>> get_static_variable_address_and_size(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_name)
    {
        auto const symbol = walker.get_symbol_info(symbol_name);
        if(!symbol.has_value())
        {
            return std::nullopt;
        }

        auto const address = symbol.value().address();
        if(!address.has_value())
        {
            throw exceptions::wide_runtime_error{std::format(L"Error: symbol name {} can't read address", symbol_name)};
        }

        auto const length = symbol.value().length();
        if(!length.has_value())
        {
            throw exceptions::wide_runtime_error{std::format(L"Error: symbol name {} can't read length", symbol_name)};
        }

        return std::make_pair(address.value(), length.value());
    }

    void throw_cant_get_field_is_null(std::wstring_view const type_name, std::wstring_view const field_name)
    {
        throw exceptions::wide_runtime_error{std::format(L"Error: symbol {0} field {1} is nullptr", type_name, field_name)};
    }

    void throw_cant_get_field_data(std::wstring_view const type_name, std::wstring_view const field_name)
    {
        throw exceptions::wide_runtime_error{std::format(L"Error: symbol {0} can't get {1} field data", type_name, field_name)};
    }

    void throw_failed_to_read_from_address(std::wstring_view const symbol_name, uint64_t const address)
    {
        throw exceptions::wide_runtime_error{std::format(L"Error: Failed to read {0} from address {1}", symbol_name, stream_hex_dump::to_hex(address))};
    }

    void throw_cant_get_symbol_field(std::wstring_view const symbol_name)
    {
        throw exceptions::wide_runtime_error{std::format(L"Error: No {} symbol field found", symbol_name)};
    }
}

