#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include <optional>

#include "mini_dump_stack_walk.h"

namespace dlg_help_utils
{
    namespace process
    {
        class process_environment_block;
    }

    namespace dbg_help
    {
        class symbol_type_info;
    }

    namespace stream_stack_dump
    {
        class mini_dump_stack_walk;
    }

    class mini_dump;
}

namespace dlg_help_utils::stream_utils
{
    [[nodiscard]] std::optional<std::tuple<size_t, MINIDUMP_DIRECTORY const&>> find_stream_for_type(mini_dump const& dump_file, MINIDUMP_STREAM_TYPE type);
    [[nodiscard]] std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> find_field_in_type(dbg_help::symbol_type_info type, std::wstring_view field_name);
    [[nodiscard]] std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> get_field_pointer_type_and_value(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& pointer_type, uint64_t memory_address, size_t index = 0);
    [[nodiscard]] std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> get_field_pointer_array_type_and_value(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& array_type, uint64_t memory_address, size_t index = 0);
    [[nodiscard]] std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> find_field_type_and_offset_in_type(dbg_help::symbol_type_info const& type, std::wstring_view field_name, dbg_help::sym_tag_enum tag);
    [[nodiscard]] std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> find_field_pointer_type_and_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address);
    [[nodiscard]] std::optional<uint64_t> find_field_pointer_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address);
    [[nodiscard]] std::optional<uint64_t> find_basic_type_field_address_in_type(dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address, size_t field_size);
    [[nodiscard]] std::optional<std::tuple<uint64_t, uint32_t, uint32_t>> find_bit_type_field_address_in_type(dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address, size_t field_size);
    [[nodiscard]] std::optional<std::tuple<std::unique_ptr<uint8_t[]>, uint64_t, uint64_t>> read_udt_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address);

    template<typename T, typename Rt = T>
    [[nodiscard]] std::optional<Rt> read_field_value(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const memory_address)
    {
        auto stream = walker.get_process_memory_stream(memory_address, sizeof(T));
        if(stream.eof())
        {
            return std::nullopt;
        }

        T value;
        if(stream.read(&value, sizeof T) != sizeof T)
        {
            return std::nullopt;
        }
        return static_cast<Rt>(value);
    }

    [[nodiscard]] uint32_t machine_field_size(process::process_environment_block const& peb);
    [[nodiscard]] std::optional<uint64_t> read_machine_size_field_value(process::process_environment_block const& peb, uint64_t memory_address);

    template<typename T>
    [[nodiscard]] std::optional<T> find_basic_type_field_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring_view const field_name, uint64_t const memory_address)
    {
        auto const field_address = find_basic_type_field_address_in_type(type, field_name, memory_address, sizeof(T));
        if(!field_address.has_value())
        {
            return std::nullopt;
        }

        return read_field_value<T>(walker, field_address.value());
    }

    template<typename T>
    [[nodiscard]] std::optional<T> find_basic_type_field_value_in_type(dbg_help::symbol_type_info const& type, std::wstring_view const field_name, void const* memory_address)
    {
        auto const field_address = find_basic_type_field_address_in_type(type, field_name, reinterpret_cast<uint64_t>(memory_address), sizeof(T));
        if(!field_address.has_value())
        {
            return std::nullopt;
        }

        T rv;
        memcpy(&rv, reinterpret_cast<void const*>(field_address.value()), sizeof(T));
        return rv;
    }

    template<typename T>
    [[nodiscard]] std::optional<T> find_bit_data_field_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring_view const field_name, uint64_t const memory_address)
    {
        auto const field_bit_data = find_bit_type_field_address_in_type(type, field_name, memory_address, sizeof(T));
        if(!field_bit_data.has_value())
        {
            return std::nullopt;
        }

        auto value = read_field_value<T>(walker, std::get<0>(field_bit_data.value()));
        if(!value.has_value())
        {
            return std::nullopt;
        }
        T const bit_mask = (~(std::numeric_limits<T>::max() << std::get<2>(field_bit_data.value()))) << std::get<1>(field_bit_data.value());
        auto data = value.value() &= bit_mask;
        data >>= std::get<1>(field_bit_data.value());
        return data;
    }

    template<typename T>
    [[nodiscard]] std::optional<T> find_bit_data_field_value_in_type(dbg_help::symbol_type_info const& type, std::wstring_view const field_name, void const* memory_address)
    {
        auto const field_bit_data = find_bit_type_field_address_in_type(type, field_name, reinterpret_cast<uint64_t>(memory_address), sizeof(T));
        if(!field_bit_data.has_value())
        {
            return std::nullopt;
        }

        T value;
        memcpy(&value, reinterpret_cast<void const*>(std::get<0>(field_bit_data.value())), sizeof(T));

        T const bit_mask = (~(std::numeric_limits<T>::max() << std::get<2>(field_bit_data.value()))) << std::get<1>(field_bit_data.value());
        auto data = value &= bit_mask;
        data >>= std::get<1>(field_bit_data.value());
        return data;
    }

    [[nodiscard]] std::optional<uint64_t> find_machine_size_field_value(process::process_environment_block const& peb, dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address);
    [[nodiscard]] dbg_help::symbol_type_info get_type(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& name);
    [[nodiscard]] uint64_t get_type_length(dbg_help::symbol_type_info const& type, std::wstring const& type_name);
    [[nodiscard]] uint64_t get_field_pointer_raw(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t address, dbg_help::symbol_type_info const& type, std::wstring const& type_name, std::wstring const& field_name);
    [[nodiscard]] uint64_t get_field_pointer(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t address, dbg_help::symbol_type_info const& type, std::wstring const& type_name, std::wstring const& field_name);
    [[nodiscard]] uint64_t get_field_offset(dbg_help::symbol_type_info const& symbol_type, std::wstring const& symbol_name, std::wstring const& field_name);

    [[nodiscard]] std::optional<uint64_t> read_static_variable_machine_size_value(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_name);
    [[nodiscard]] std::optional<std::pair<uint64_t,uint64_t>> get_static_variable_address_and_size(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_name);

    [[noreturn]] void throw_cant_get_field_is_null(std::wstring const& type_name, std::wstring const& field_name);
    [[noreturn]] void throw_cant_get_field_data(std::wstring const& type_name, std::wstring const& field_name);
    [[noreturn]] void throw_failed_to_read_from_address(std::wstring const& symbol_name, uint64_t address);
    [[noreturn]] void throw_cant_get_symbol_field(std::wstring const& symbol_name);

    template <typename T, typename E>
    [[nodiscard]] T get_field_value(E const& entry, std::wstring const& field_name)
    {
        auto const value = stream_utils::find_basic_type_field_value_in_type<T>(entry.walker(), entry.symbol_type(), field_name, entry.symbol_address());
        if(!value.has_value())
        {
            throw_cant_get_field_data(E::symbol_name, field_name);
        }

        return value.value();
    }

    template <typename T, typename E>
    T get_bit_field_value(E const& entry, std::wstring const& field_name)
    {
        auto const value = stream_utils::find_bit_data_field_value_in_type<T>(entry.walker(), entry.symbol_type(), field_name, entry.symbol_address());
        if(!value.has_value())
        {
            throw_cant_get_field_data(E::symbol_name, field_name);
        }

        return value.value();
    }

    template <typename T, typename E>
    [[nodiscard]] T get_field_value_from_buffer(E const& entry, std::wstring const& field_name, void const* buffer)
    {
        auto const value = stream_utils::find_basic_type_field_value_in_type<T>(entry.symbol_type(), field_name, buffer);
        if(!value.has_value())
        {
            stream_utils::throw_cant_get_field_data(E::symbol_name, field_name);
        }

        return value.value();
    }

    template <typename T, typename E>
    T get_bit_field_value_from_buffer(E const& entry, std::wstring const& field_name, void const* buffer)
    {
        auto const value = stream_utils::find_bit_data_field_value_in_type<T>(entry.symbol_type(), field_name, buffer);
        if(!value.has_value())
        {
            throw_cant_get_field_data(E::symbol_name, field_name);
        }

        return value.value();
    }

    template <typename E>
    uint64_t get_field_pointer(E const& entry, std::wstring const& field_name)
    {
        return stream_utils::get_field_pointer(entry.walker(), entry.symbol_address(), entry.symbol_type(), E::symbol_name, field_name);
    }

    template <typename E>
    uint64_t get_field_address(E const& entry, std::wstring const& field_name)
    {
        return entry.symbol_address() + stream_utils::get_field_offset(entry.symbol_type(), E::symbol_name, field_name);
    }

    template <typename E>
    [[nodiscard]] uint64_t get_machine_size_field_value(E const& entry, std::wstring const& field_name)
    {
        auto const value = find_machine_size_field_value(entry.peb(), entry.symbol_type(), field_name, entry.symbol_address());
        if(!value.has_value())
        {
            throw_cant_get_field_data(E::symbol_name, field_name);
        }

        return value.value();
    }
}
