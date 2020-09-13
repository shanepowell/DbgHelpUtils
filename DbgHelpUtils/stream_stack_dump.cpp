#include "stream_stack_dump.h"

#include <filesystem>


#include "function_table_stream.h"
#include "memory64_list_stream.h"
#include "memory_list_stream.h"
#include "mini_dump_stack_walk.h"
#include "module_list_stream.h"
#include "pe_file_memory_mapping.h"
#include "stream_hex_dump.h"
#include "stream_thread_context.h"
#include "symbol_engine.h"
#include "system_info_stream.h"
#include "unloaded_module_list_stream.h"

using namespace std::string_view_literals;

namespace dlg_help_utils::stream_stack_dump
{
    namespace
    {
        template <typename T>
        void generate_hex_dump_address(std::wostream& os, T const stack_address, T address,
                                       std::optional<dbg_help::symbol_address_info> info)
        {
            if (info && info->in_line)
            {
                auto const digit_print_size = (sizeof(T) * 2) + 2;
                os << std::setw(digit_print_size + (stack_address > 0 ? digit_print_size + 2 : 0)) << std::setfill(L' ')
                    << L"(Inline)";
            }
            else
            {
                if (stack_address > 0)
                {
                    os << stream_hex_dump::to_hex_full(stack_address) << L": ";
                }

                os << stream_hex_dump::to_hex_full(address);
            }

            if (info && info->found)
            {
                std::filesystem::path const p{info->module_name};
                auto filename = p.filename().wstring();
                auto const last_index = filename.find_last_of(L'.');
                if (last_index != std::wstring::npos)
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
        void generate_hex_dump_stack_raw(std::wostream& os, dbg_help::symbol_engine& symbol_engine,
                                         uint64_t const stack_start_address, T const* stack, size_t const stack_size,
                                         size_t const indent, module_list_stream const& module_list,
                                         unloaded_module_list_stream const& unloaded_module_list)
        {
            const std::wstring indent_str(indent, L' ');

            for (size_t index = 0; index < stack_size; ++index)
            {
                os << indent_str << stream_hex_dump::to_hex(index, 2, L'0', false) << L' ';
                generate_hex_dump_address(os, stack_start_address + (index * sizeof(T)), stack[index],
                                          mini_dump_stack_walk::find_symbol_info(
                                              stack[index], module_list, unloaded_module_list, symbol_engine));
                os << L'\n';
            }
        }
    }


    void hex_dump_stack(std::wostream& os, mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine,
                        uint64_t const stack_start_address, void const* stack, const size_t stack_size,
                        stream_thread_context const& thread_context, const size_t indent)
    {
        const std::wstring indent_str(indent, L' ');

        memory_list_stream const memory_list{mini_dump};
        memory64_list_stream const memory64_list{mini_dump};
        function_table_stream const function_table{mini_dump};
        module_list_stream const module_list{mini_dump};
        unloaded_module_list_stream const unloaded_module_list{mini_dump};
        pe_file_memory_mapping pe_file_memory_mappings{};
        mini_dump_stack_walk walk_callback{
            stack_start_address, stack, stack_size, memory_list, memory64_list, function_table, module_list,
            unloaded_module_list, pe_file_memory_mappings, symbol_engine
        };

        size_t index = 0;

        for (auto const& entry : dbg_help::symbol_engine::stack_walk(thread_context, walk_callback))
        {
            os << indent_str << stream_hex_dump::to_hex(index, 2, L'0', false) << L' ';

            if (thread_context.x86_thread_context_available() || thread_context.wow64_thread_context_available())
            {
                generate_hex_dump_address(os, static_cast<uint32_t>(entry.stack), static_cast<uint32_t>(entry.address),
                                          entry);
            }
            else if (thread_context.x64_thread_context_available())
            {
                generate_hex_dump_address(os, entry.stack, entry.address, entry);
            }

            os << L'\n';
            ++index;
        }
    }

    void hex_dump_stack_raw(std::wostream& os, mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine,
                            uint64_t const stack_start_address, uint64_t const* stack, size_t const stack_size,
                            size_t const indent)
    {
        module_list_stream const module_list{mini_dump};
        unloaded_module_list_stream const unloaded_module_list{mini_dump};
        generate_hex_dump_stack_raw(os, symbol_engine, stack_start_address, stack, stack_size, indent, module_list,
                                    unloaded_module_list);
    }

    void hex_dump_address(std::wostream& os, mini_dump const& mini_dump, module_list_stream const& module_list,
                          unloaded_module_list_stream const& unloaded_module_list,
                          dbg_help::symbol_engine& symbol_engine, uint64_t const address, size_t const indent)
    {
        const std::wstring indent_str(indent, L' ');
        os << indent_str;

        system_info_stream const system_info{mini_dump};
        if (system_info.is_x86())
        {
            generate_hex_dump_address(os, static_cast<uint32_t>(0), static_cast<uint32_t>(address),
                                      mini_dump_stack_walk::find_symbol_info(
                                          address, module_list, unloaded_module_list, symbol_engine));
        }
        else if (system_info.is_x64())
        {
            generate_hex_dump_address(os, static_cast<uint64_t>(0), address,
                                      mini_dump_stack_walk::find_symbol_info(
                                          address, module_list, unloaded_module_list, symbol_engine));
        }

        os << L'\n';
    }
};
