﻿#pragma once
#include <iosfwd>
#include <vector>

#include "symbol_address_info.h"
#include "tagged_bool.h"

namespace dlg_help_utils
{
    class stream_thread_context;
    class unloaded_module_list_stream;
    class module_list_stream;

    namespace dbg_help
    {
        class symbol_engine;
    }

    class mini_dump;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;

    enum class dump_stack_options : uint8_t
    {
        DisplayStackVariables = 0x01,
        DisplayStackParameters = 0x02
    };

    using is_x86_target_t = tagged_bool<struct is_x86_target_type>;

    struct stack_function_call_entry
    {
        size_t index;
        dbg_help::symbol_address_info  symbol_info;
        std::wstring line;
    };

    generator<stack_function_call_entry> dump_stack(mini_dump_memory_walker& walker
        , dbg_help::symbol_engine& symbol_engine
        , stream_thread_context const& thread_context
        , size_t indent);
    generator<std::wstring> dump_stack(mini_dump_memory_walker const& walker
        , std::vector<uint64_t> const& stack
        , is_x86_target_t is_x86_target
        , size_t indent);
    generator<std::wstring> dump_stack_raw(mini_dump const& mini_dump
        , dbg_help::symbol_engine& symbol_engine
        , uint64_t stack_start_address
        , uint64_t const* stack
        , size_t stack_size
        , is_x86_target_t is_x86_target
        , size_t indent);
    void dump_stack_to_stream(std::wostream& os
        , mini_dump const& mini_dump
        , dbg_help::symbol_engine& symbol_engine
        , uint64_t stack_start_address
        , void const* stack
        , size_t stack_size
        , stream_thread_context const& thread_context
        , size_t indent
        , dump_stack_options options);
    void dump_stack_to_stream(std::wostream& os
        , mini_dump_memory_walker const& walker
        , std::vector<uint64_t> const& stack
        , is_x86_target_t is_x86_target
        , size_t indent);
    void dump_stack_to_stream_raw(std::wostream& os
        , mini_dump const& mini_dump
        , dbg_help::symbol_engine& symbol_engine
        , uint64_t stack_start_address
        , uint64_t const* stack
        , size_t stack_size
        , is_x86_target_t is_x86_target
        , size_t indent);
    void dump_address_to_stream(std::wostream& os
        , mini_dump const& mini_dump
        , module_list_stream const& module_list
        , unloaded_module_list_stream const& unloaded_module_list
        , dbg_help::symbol_engine& symbol_engine
        , uint64_t address
        , size_t indent);
    std::wstring dump_stack_frame(dbg_help::symbol_address_info const& info, is_x86_target_t is_x86_address);

    namespace resources
    {
        std::wstring get_inline_string();
        std::wstring get_parameters_title();
        std::wstring get_local_variables_title();
        std::wstring get_symbol_name_prefix();
        std::wstring get_symbol_displacement_prefix();
        std::wstring get_module_displacement_prefix();
        std::wstring get_unknown_variable_type();
        std::wstring get_file_name_line_number_separator();
        std::wstring get_failed_to_find_address_prefix();
        std::wstring get_failed_to_find_address_name_address_separator();
        std::wstring get_failed_to_find_address_postfix();
    }
};
