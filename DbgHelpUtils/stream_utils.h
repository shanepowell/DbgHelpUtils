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
    std::optional<std::tuple<size_t, MINIDUMP_DIRECTORY const&>> find_stream_for_type(mini_dump const& dump_file, MINIDUMP_STREAM_TYPE type);
    std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> find_field_in_type(dbg_help::symbol_type_info type, std::wstring_view field_name);
    std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> get_field_pointer_type_and_value(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& pointer_type, uint64_t memory_address, size_t index = 0);
    std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> get_field_pointer_array_type_and_value(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& array_type, uint64_t memory_address, size_t index = 0);
    std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> find_field_type_and_offset_in_type(dbg_help::symbol_type_info const& type, std::wstring_view field_name, dbg_help::sym_tag_enum tag);
    std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> find_field_pointer_type_and_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address);
    std::optional<uint64_t> find_field_pointer_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address);
    std::optional<uint64_t> find_basic_type_field_address_in_type(dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address, size_t field_size);
    std::optional<std::tuple<std::unique_ptr<char[]>, uint64_t, uint64_t>> read_udt_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address);

    template<typename T, typename Rt = T>
    std::optional<Rt> read_field_value(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const memory_address)
    {
        auto const* memory = walker.get_process_memory(memory_address, sizeof(T));
        if(memory == nullptr)
        {
            return std::nullopt;
        }

        return static_cast<Rt>(*static_cast<T const*>(memory));
    }

    uint32_t machine_field_size(process::process_environment_block const& peb);
    std::optional<uint64_t> read_machine_size_field_value(process::process_environment_block const& peb, uint64_t memory_address);

    template<typename T>
    std::optional<T> find_basic_type_field_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring_view const field_name, uint64_t const memory_address)
    {
        auto const field_address = find_basic_type_field_address_in_type(type, field_name, memory_address, sizeof(T));
        if(!field_address.has_value())
        {
            return std::nullopt;
        }

        return read_field_value<T>(walker, field_address.value());
    }

    template<typename T>
    std::optional<T> find_basic_type_field_value_in_type(dbg_help::symbol_type_info const& type, std::wstring_view const field_name, void const* memory_address)
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

    std::optional<uint64_t> find_machine_size_field_value(process::process_environment_block const& peb, dbg_help::symbol_type_info const& type, std::wstring_view field_name, uint64_t memory_address);
    [[nodiscard]] dbg_help::symbol_type_info get_type(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& name);
    [[nodiscard]] uint64_t get_type_length(dbg_help::symbol_type_info const& type, std::wstring const& type_name);
    [[nodiscard]] uint64_t get_field_pointer_raw(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t address, dbg_help::symbol_type_info const& type, std::wstring const& type_name, std::wstring const& field_name);
    [[nodiscard]] uint64_t get_field_pointer(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t address, dbg_help::symbol_type_info const& type, std::wstring const& type_name, std::wstring const& field_name);
    [[nodiscard]] uint64_t get_field_offset(dbg_help::symbol_type_info const& symbol_type, std::wstring const& symbol_name, std::wstring const& field_name);

    std::optional<uint64_t> read_static_variable_value(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_name);

    [[noreturn]] void throw_cant_get_field_is_null(std::wstring const& type_name, std::wstring const& field_name);
    [[noreturn]] void throw_cant_get_field_data(std::wstring const& type_name, std::wstring const& field_name);
    [[noreturn]] void throw_failed_to_read_from_address(std::wstring const& symbol_name, uint64_t address);
    [[noreturn]] void throw_cant_get_symbol_field(std::wstring const& symbol_name);
}
