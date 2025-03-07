﻿#include "stream_stack_dump.h"

#include <filesystem>

#include "function_table_stream.h"
#include "memory64_list_stream.h"
#include "memory_list_stream.h"
#include "mini_dump_memory_walker.h"
#include "module_list_stream.h"
#include "pe_file_memory_mapping.h"
#include "register_names.h"
#include "stream_hex_dump.h"
#include "stream_thread_context.h"
#include "symbol_engine.h"
#include "symbol_type_utils.h"
#include "system_info_stream.h"
#include "unloaded_module_list_stream.h"

using namespace std::string_view_literals;

namespace dlg_help_utils::stream_stack_dump
{
    namespace
    {
        template <typename T>
        void generate_hex_dump_address(std::wostream& os, T const stack_address, T address, std::optional<dbg_help::symbol_address_info> const& info, size_t const hex_length = sizeof(T) * 2)
        {
            if (info && info->in_line)
            {
                auto const digit_print_size = hex_length + 2;
                os << std::setw(digit_print_size + (stack_address > 0 ? digit_print_size + 2 : 0)) << std::setfill(L' ')
                    << L"(Inline)";
            }
            else
            {
                if (stack_address > 0)
                {
                    os << stream_hex_dump::to_hex(stack_address, hex_length) << L": ";
                }

                os << stream_hex_dump::to_hex(address, hex_length);
            }

            if (info && info->found)
            {
                std::filesystem::path const p{info->module_name};
                auto filename = p.filename().wstring();
                if (auto const last_index = filename.find_last_of(L'.'); last_index != std::wstring::npos)
                {
                    filename = filename.substr(0, last_index);
                }
                os << L' ' << filename;

                if (!info->symbol_name.empty())
                {
                    os << L'!' << info->symbol_name;
                    if (info->symbol_displacement > 0)
                    {
                        os << L'+' << stream_hex_dump::to_hex(info->symbol_displacement);
                    }
                    if (!info->file_name.empty())
                    {
                        os << L" [" << info->file_name << L'@' << info->line_number << L']';
                    }
                }
                else
                {
                    os << L'+' << stream_hex_dump::to_hex(info->module_displacement);
                }
            }
        }

        template <typename T>
        void generate_hex_dump_stack_raw(std::wostream& os
            , dbg_help::symbol_engine& symbol_engine
            , uint64_t const stack_start_address
            , T const* stack
            , size_t const stack_size
            , size_t const indent
            , module_list_stream const& module_list
            , unloaded_module_list_stream const& unloaded_module_list
            , size_t const hex_length)
        {
            const std::wstring indent_str(indent, L' ');

            for (size_t index = 0; index < stack_size; ++index)
            {
                os << indent_str << stream_hex_dump::to_hex(index, 2, L'0', write_header_t{false}) << L' ';
                generate_hex_dump_address(os
                    , stack_start_address == 0 ? 0 : stack_start_address + (index * sizeof(T))
                    , stack[index]
                    , mini_dump_memory_walker::find_symbol_info(stack[index], module_list, unloaded_module_list, symbol_engine)
                    , hex_length);
                os << L'\n';
            }
        }

        size_t get_hex_length(is_x86_target_t const is_x86_target)
        {
            if(is_x86_target)
            {
                return sizeof(uint32_t) * 2;
            }

            return sizeof(uint64_t) * 2;
        }

        template <typename T>
        void generate_hex_dump_local_variable(std::wostream& os, mini_dump_memory_walker const& walker, dbg_help::local_variable const& local_variable, size_t const hex_length = sizeof(T) * 2)
        {
            auto const name = symbol_type_utils::get_symbol_type_friendly_name(local_variable.symbol_info);
            os << std::format(L"\n          {0}", name);
            if(local_variable.registry_value.has_value() || local_variable.frame_data.has_value())
            {
                os << (local_variable.registry_value 
                        ? (local_variable.frame_data 
                            ? std::format(L" [{0}{1:+}]({2})", register_names::get_register_name(local_variable.registry_value->register_type), local_variable.frame_data->data_offset, stream_hex_dump::to_hex(local_variable.frame_data->data_address, static_cast<std::streamsize>(hex_length)))
                            : std::format(L" [{}]", register_names::get_register_name(local_variable.registry_value->register_type))
                          )
                        : std::format(L" ({})", stream_hex_dump::to_hex(local_variable.frame_data->data_address, static_cast<std::streamsize>(hex_length)))
                      );

                if(local_variable.frame_data)
                {
                    auto const stream = walker.get_process_memory_stream(local_variable.frame_data->data_address, local_variable.frame_data->data_size);
                    if(stream.eof())
                    {
                        os << std::format(L"\nFailed to find {0} address [{1}] in dump file\n", name, stream_hex_dump::to_hex_full(local_variable.frame_data->data_address));
                        return;
                    }
                    dump_variable_symbol_at(os, walker, local_variable.symbol_info, local_variable.symbol_info, local_variable.frame_data->data_address, stream, 12, 0, symbol_type_utils::dump_variable_symbol_options::NoHeader);
                }
                else if(local_variable.registry_value)
                {
                    mini_dump_memory_stream const stream{&local_variable.registry_value->value, local_variable.registry_value->value_size};
                    dump_variable_symbol_at(os, walker, local_variable.symbol_info, local_variable.symbol_info, 0, stream, 12, 0, symbol_type_utils::dump_variable_symbol_options::NoHeader);
                }
            }
            else
            {
                os << L": <unknown>";
            }
        }

        template <typename T>
        void generate_hex_dump_local_variables(std::wostream& os, std::wstring_view const title, mini_dump_memory_walker const& walker, std::vector<dbg_help::local_variable> const& local_variables)
        {
            if(!local_variables.empty())
            {
                os << std::format(L"\n{}:", title);
                for (auto const& local_variable : local_variables)
                {
                    generate_hex_dump_local_variable<T>(os, walker, local_variable);
                }
            }
        }
    }


    void hex_dump_stack(std::wostream& os
        , mini_dump const& mini_dump
        , dbg_help::symbol_engine& symbol_engine
        , uint64_t const stack_start_address
        , void const* stack
        , const size_t stack_size
        , stream_thread_context const& thread_context
        , const size_t indent
        , dump_stack_options::options const options)
    {
        const std::wstring indent_str(indent, L' ');

        memory_list_stream const memory_list{mini_dump};
        memory64_list_stream const memory64_list{mini_dump};
        function_table_stream const function_table{mini_dump};
        module_list_stream const module_list{mini_dump};
        unloaded_module_list_stream const unloaded_module_list{mini_dump};
        pe_file_memory_mapping pe_file_memory_mappings{};
        mini_dump_memory_walker const walker
            {
                stack_start_address,
                stack,
                stack_size,
                memory_list,
                memory64_list,
                function_table,
                module_list,
                unloaded_module_list,
                pe_file_memory_mappings,
                symbol_engine
            };

        auto const display_parameters = (options & dump_stack_options::DisplayStackParameters) == dump_stack_options::DisplayStackParameters;
        auto const display_variables = (options & dump_stack_options::DisplayStackVariables) == dump_stack_options::DisplayStackVariables;

        for (size_t index = 0; auto const& entry : symbol_engine.stack_walk(thread_context))
        {
            os << indent_str << stream_hex_dump::to_hex(index, 2, L'0', write_header_t{false}) << L' ';
            auto constexpr parameters_title = L"Parameters"sv;
            auto constexpr local_variables_title = L"Local Variables"sv;

            if (thread_context.x86_thread_context_available() || thread_context.wow64_thread_context_available())
            {
                generate_hex_dump_address(os, static_cast<uint32_t>(entry.stack), static_cast<uint32_t>(entry.address), entry);
                if(display_parameters)
                {
                    generate_hex_dump_local_variables<uint32_t>(os, parameters_title, walker, entry.parameters);
                }
                if(display_variables)
                {
                    generate_hex_dump_local_variables<uint32_t>(os, local_variables_title, walker, entry.local_variables);
                }
            }
            else if (thread_context.x64_thread_context_available())
            {
                generate_hex_dump_address(os, entry.stack, entry.address, entry);
                if(display_parameters)
                {
                    generate_hex_dump_local_variables<uint64_t>(os, parameters_title, walker, entry.parameters);
                }
                if(display_variables)
                {
                    generate_hex_dump_local_variables<uint64_t>(os, local_variables_title, walker, entry.local_variables);
                }
            }

            os << L'\n';
            ++index;
        }
    }

    void hex_dump_stack(std::wostream& os
        , mini_dump_memory_walker const& walker
        , std::vector<uint64_t> const& stack
        , is_x86_target_t const is_x86_target
        , size_t const indent)
    {
        generate_hex_dump_stack_raw(os
            , walker.symbol_engine()
            , 0
            , stack.data()
            , stack.size()
            , indent
            , walker.module_list()
            , walker.unloaded_module_list()
            , get_hex_length(is_x86_target));
    }

    void hex_dump_stack_raw(std::wostream& os
        , mini_dump const& mini_dump
        , dbg_help::symbol_engine& symbol_engine
        , uint64_t const stack_start_address
        , uint64_t const* stack
        , size_t const stack_size
        , is_x86_target_t const is_x86_target
        , size_t const indent)
    {
        module_list_stream const module_list{mini_dump};
        unloaded_module_list_stream const unloaded_module_list{mini_dump};
        generate_hex_dump_stack_raw(os
            , symbol_engine
            , stack_start_address
            , stack
            , stack_size
            , indent
            , module_list
            , unloaded_module_list
            , get_hex_length(is_x86_target));
    }

    void hex_dump_address(std::wostream& os
        , mini_dump const& mini_dump
        , module_list_stream const& module_list
        , unloaded_module_list_stream const& unloaded_module_list
        , dbg_help::symbol_engine& symbol_engine
        , uint64_t const address
        , size_t const indent)
    {
        const std::wstring indent_str(indent, L' ');
        os << indent_str;

        if (system_info_stream const system_info{mini_dump}; system_info.is_x86())
        {
            generate_hex_dump_address(os
                , static_cast<uint32_t>(0)
                , static_cast<uint32_t>(address)
                , mini_dump_memory_walker::find_symbol_info(address
                    , module_list
                    , unloaded_module_list
                    , symbol_engine));
        }
        else if (system_info.is_x64())
        {
            generate_hex_dump_address(os, static_cast<uint64_t>(0)
                , address
                , mini_dump_memory_walker::find_symbol_info(address
                    , module_list
                    , unloaded_module_list
                    , symbol_engine));
        }

        os << L'\n';
    }

    std::wstring hex_dump_stack_frame(dbg_help::symbol_address_info const& info, is_x86_target_t const is_x86_address)
    {
        std::wstringstream ss;
        if(is_x86_address)
        {
            generate_hex_dump_address(ss, static_cast<uint32_t>(info.stack), static_cast<uint32_t>(info.address), info);
        }
        else
        {
            generate_hex_dump_address(ss, info.stack, info.address, info);
        }

        return std::move(ss).str();
    }
};
