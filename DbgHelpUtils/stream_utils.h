#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include <optional>
#include <string>

#include "mini_dump_stack_walk.h"

namespace dlg_help_utils
{
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

    std::optional<DWORD64> find_field_pointer_value_in_type(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type, std::wstring const& field_name, DWORD64 memory_address);

    template<typename T, typename Rt = T>
    std::optional<Rt> read_field_value(stream_stack_dump::mini_dump_stack_walk const& walker, DWORD64 const memory_address)
    {
        auto const* memory = walker.get_process_memory(memory_address, sizeof(T));
        if(memory == nullptr)
        {
            return std::nullopt;
        }

        return static_cast<Rt>(*static_cast<T const*>(memory));
    }
}
