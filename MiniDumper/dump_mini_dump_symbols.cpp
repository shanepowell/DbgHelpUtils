#include "dump_mini_dump_symbols.h"

#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <iostream>

#include "dump_file_options.h"
#include "DbgHelpUtils/common_symbol_names.h"
#include "DbgHelpUtils/print_utils.h"
#include "DbgHelpUtils/function_table_stream.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/memory64_list_stream.h"
#include "DbgHelpUtils/memory_list_stream.h"
#include "DbgHelpUtils/mini_dump_stack_walk.h"
#include "DbgHelpUtils/module_list_stream.h"
#include "DbgHelpUtils/pe_file_memory_mapping.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_utils.h"
#include "DbgHelpUtils/symbol_type_info.h"
#include "DbgHelpUtils/symbol_type_utils.h"
#include "DbgHelpUtils/thread_ex_list_stream.h"
#include "DbgHelpUtils/thread_list_stream.h"
#include "DbgHelpUtils/thread_names_list_stream.h"
#include "DbgHelpUtils/unloaded_module_list_stream.h"

using namespace std;
using namespace dlg_help_utils;

void dump_mini_dump_symbol_type(mini_dump const& mini_dump, std::wstring const& type_name, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    memory_list_stream const memory_list{mini_dump};
    memory64_list_stream const memory64_list{ mini_dump };
    function_table_stream const function_table{ mini_dump };
    module_list_stream const module_list{ mini_dump };
    unloaded_module_list_stream const unloaded_module_list{ mini_dump };
    pe_file_memory_mapping pe_file_memory_mappings{};
    stream_stack_dump::mini_dump_stack_walk const walker{
        0, nullptr, 0, memory_list, memory64_list, function_table, module_list,
        unloaded_module_list, pe_file_memory_mappings, symbol_engine
    };

    if(auto const symbol_info = walker.get_type_info(type_name); symbol_info.has_value())
    {
        wcout << "Symbol Type [" << type_name << "] found:\n";
        dump_symbol_type(symbol_info.value(), options);
    }
    else
    {
        wcout << "Symbol Type [" << type_name << "] not found\n";
    }
}

void dump_mini_dump_module_symbol_types(mini_dump const& mini_dump, std::wstring const& module_name, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    memory_list_stream const memory_list{mini_dump};
    memory64_list_stream const memory64_list{ mini_dump };
    function_table_stream const function_table{ mini_dump };
    module_list_stream const module_list{ mini_dump };
    unloaded_module_list_stream const unloaded_module_list{ mini_dump };
    pe_file_memory_mapping pe_file_memory_mappings{};
    stream_stack_dump::mini_dump_stack_walk const walker{
        0, nullptr, 0, memory_list, memory64_list, function_table, module_list,
        unloaded_module_list, pe_file_memory_mappings, symbol_engine
    };

    for (auto first = true; auto const& type : walker.module_types(module_name))
    {
        if(first)
        {
            first = false;
        }
        else
        {
            wcout << '\n';
        }

        dump_symbol_type(type, options);
    }
}

template<typename T>
void dump_address_type_array(std::wostream& os, std::wstring const& dt, T const* values, size_t const memory_size, size_t const elements_per_line, size_t const element_width, size_t const indent, bool const dump_hex)
{
    os << dt << " array @ [" << stream_hex_dump::to_hex_full(values) << "] for [" << memory_size / sizeof(T) << "] elements:\n";
    print_utils::print_array_lines(wcout, values, memory_size / sizeof(T), elements_per_line, element_width, indent, dump_hex);
    os << '\n';
}

void dump_mini_dump_address(mini_dump const& mini_dump, std::wstring const& address, [[maybe_unused]] dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    memory_list_stream const memory_list{mini_dump};
    memory64_list_stream const memory64_list{ mini_dump };
    function_table_stream const function_table{ mini_dump };
    module_list_stream const module_list{ mini_dump };
    unloaded_module_list_stream const unloaded_module_list{ mini_dump };
    pe_file_memory_mapping pe_file_memory_mappings{};
    stream_stack_dump::mini_dump_stack_walk const walker{
        0, nullptr, 0, memory_list, memory64_list, function_table, module_list,
        unloaded_module_list, pe_file_memory_mappings, symbol_engine
    };

    if(auto [memory_pointer, symbol_type, memory_size, dt] = symbol_type_utils::parse_address(address); !symbol_type.empty())
    {
        symbol_type_utils::dump_variable_type_at(wcout, mini_dump, symbol_engine, symbol_type, memory_pointer);
    }
    else
    {
        auto const find_limit = memory_size == 0;
        if(find_limit)
        {
            memory_size = std::numeric_limits<uint64_t>::max();
        }

        auto const* memory = walker.get_process_memory_range(memory_pointer, memory_size);
        if(memory == nullptr)
        {
            wcout << "Can't find memory address [" << stream_hex_dump::to_hex_full(memory_pointer) << "]\n";
        }

        if(dt.empty())
        {
            hex_dump::hex_dump(wcout, memory, memory_size);
        }
        else if(boost::iequals(dt, L"str"s))
        {
            wcout << "string @ [" << stream_hex_dump::to_hex_full(memory_pointer) << "]:\n";
            print_utils::print_str(wcout, static_cast<char const*>(memory), memory_size, find_limit);
            wcout << '\n';
        }
        else if(boost::iequals(dt, L"wstr"s))
        {
            wcout << "unicode string @ [" << stream_hex_dump::to_hex_full(memory_pointer) << "]:\n";
            print_utils::print_str(wcout, static_cast<wchar_t const*>(memory), memory_size / sizeof(wchar_t), find_limit);
            wcout << '\n';
        }
        else if(boost::iequals(dt, L"astr"s))
        {
            wcout << "string array @ [" << stream_hex_dump::to_hex_full(memory_pointer) << "]:\n";
            print_utils::print_array_str(wcout, static_cast<char const*>(memory), memory_size, 2);
            wcout << '\n';
        }
        else if(boost::iequals(dt, L"awstr"s))
        {
            wcout << "unicode string array @ [" << stream_hex_dump::to_hex_full(memory_pointer) << "]:\n";
            print_utils::print_array_str(wcout, static_cast<wchar_t const*>(memory), memory_size / sizeof(wchar_t), 2);
            wcout << '\n';
        }
        else
        {
            auto display_hex = false;
            if(dt.ends_with(L",h"s) || dt.ends_with(L",H"s))
            {
                dt = dt.substr(0, dt.size() - 2);
                display_hex = true;
            }
            if(boost::iequals(dt, L"uint8"s))
            {
                dump_address_type_array(wcout, dt, static_cast<uint8_t const*>(memory), memory_size, 16, 6, 2, display_hex);
            }
            else if(boost::iequals(dt, L"int8"s))
            {
                dump_address_type_array(wcout, dt, static_cast<int8_t const*>(memory), memory_size, 16, 6, 2, display_hex);
            }
            else if(boost::iequals(dt, L"uint16"s))
            {
                dump_address_type_array(wcout, dt, static_cast<uint16_t const*>(memory), memory_size, 8, 8, 2, display_hex);
            }
            else if(boost::iequals(dt, L"int16"s))
            {
                dump_address_type_array(wcout, dt, static_cast<int16_t const*>(memory), memory_size, 8, 8, 2, display_hex);
            }
            else if(boost::iequals(dt, L"uint32"s))
            {
                dump_address_type_array(wcout, dt, static_cast<uint32_t const*>(memory), memory_size, 4, 14, 2, display_hex);
            }
            else if(boost::iequals(dt, L"int32"s))
            {
                dump_address_type_array(wcout, dt, static_cast<int32_t const*>(memory), memory_size, 4, 14, 2, display_hex);
            }
            else if(boost::iequals(dt, L"uint64"s))
            {
                dump_address_type_array(wcout, dt, static_cast<uint64_t const*>(memory), memory_size, 2, 24, 2, display_hex);
            }
            else if(boost::iequals(dt, L"int64"s))
            {
                dump_address_type_array(wcout, dt, static_cast<int64_t const*>(memory), memory_size, 2, 24, 2, display_hex);
            }
            else if(boost::iequals(dt, L"float"s))
            {
                dump_address_type_array(wcout, dt, static_cast<float const*>(memory), memory_size, 2, 24, 2, display_hex);
            }
            else if(boost::iequals(dt, L"double"s))
            {
                dump_address_type_array(wcout, dt, static_cast<double const*>(memory), memory_size, 2, 24, 2, display_hex);
            }
            else
            {
                wcout << "Invalid dt [" << dt << "]\n";
            }
        }
    }
}

void dump_symbol_type(dbg_help::symbol_type_info const& value, [[maybe_unused]] dump_file_options const& options, size_t const base_offset, size_t const indent)
{
    auto const tag_data = value.sym_tag();
    auto const offset_data = value.offset();

    wcout << std::wstring(indent, ' ');

    auto const bit_position_data = value.bit_position();

    {
        std::wostringstream ss;
        if(offset_data.has_value())
        {
            ss << L'+' << stream_hex_dump::to_hex_full(base_offset + offset_data.value());
        }

        if(auto const str = std::move(ss).str(); !bit_position_data.has_value())
        {
            wcout << str;
        }
        else if(options.debug_type_data())
        {
            wcout << str << ' ';
        }
        else
        {
            wcout << std::wstring(str.size() + 1, ' ');
        }
    }

    auto name = symbol_type_utils::get_symbol_type_friendly_name(value);
    wcout << ' ' << name;

    if(bit_position_data.has_value())
    {
        auto const length_data = value.length();
        auto const is_bits = length_data.value_or(1) > 1;
        wcout << " bit" << (is_bits ? "s" : "") << " " << bit_position_data.value();
        uint64_t bit_mask;
        if(is_bits)
        {
            wcout << "-" << bit_position_data.value() + length_data.value() - 1;
            bit_mask = (~(std::numeric_limits<uint64_t>::max() << length_data.value())) << bit_position_data.value();
        }
        else
        {
            bit_mask = 0x01ULL << bit_position_data.value();
        }

        wcout << " (" << stream_hex_dump::to_hex(bit_mask) << ")";
    }
    else
    {
        if(auto const data = value.length(); data.has_value())
        {
            wcout << ", Length: " << data.value();
        }
    }

    if(options.debug_type_data())
    {
        wcout << ", SymIndex: " << value.sym_index();

        if(tag_data.has_value())
        {
            wcout << ", tag: " << symbol_type_utils::sym_tag_to_string(tag_data.value());
        }

        if(auto const data = value.base_type(); data.has_value())
        {
            wcout << ", BaseType: " << symbol_type_utils::basic_type_to_string(data.value());
        }
        
        if(auto const data = value.data_kind(); data.has_value())
        {
            wcout << ", DataKind: " << symbol_type_utils::data_kind_convention_to_string(data.value());
        }

        if(auto const data = value.address_offset(); data.has_value())
        {
            wcout << ", AddressOffset: " << data.value();
        }

        if(auto const data = value.array_index_typeid(); data.has_value())
        {
            wcout << ", ArrayIndexTypeId: " << data.value();
        }

        if(auto const data = value.array_count(); data.has_value())
        {
            wcout << ", ArrayCount: " << data.value();
        }

        if(auto const data = value.bit_position(); data.has_value())
        {
            wcout << ", BitPosition: " << data.value();
        }

        if(auto const data = value.is_virtual_base_class(); data.has_value())
        {
            wcout << ", IsVirtualBaseClass: " << std::boolalpha << data.value();
        }

        if(auto const data = value.virtual_base_pointer_offset(); data.has_value())
        {
            wcout << ", VirtualBasePointerOffset: " << data.value();
        }

        if(auto const data = value.class_parent_id(); data.has_value())
        {
            wcout << ", ClassParentId: " << data.value();
        }

        if(auto const data = value.nested(); data.has_value())
        {
            wcout << ", Nested: " << data.value();
        }

        if(auto const data = value.lexical_parent(); data.has_value())
        {
            wcout << ", LexicalParent: " << data.value();
        }


        if(auto const data = value.address(); data.has_value())
        {
            wcout << ", Address: " << data.value();
        }

        if(auto const data = value.virtual_base_offset(); data.has_value())
        {
            wcout << ", VirtualBaseOffset: " << data.value();
        }


        if(auto const data = value.virtual_base_displacement_table_index(); data.has_value())
        {
            wcout << ", VirtualBaseDisplacementTableIndex: " << data.value();
        }


        if(auto const data = value.is_reference(); data.has_value())
        {
            wcout << ", IsReference: " << std::boolalpha << data.value();
        }


        if(auto const data = value.indirect_virtual_base_class(); data.has_value())
        {
            wcout << ", IndirectVirtualBaseClass: " << std::boolalpha << data.value();
        }


        if(auto const data = value.const_value(); data.has_value())
        {
            wcout << ", ConstValue: " << static_cast<_bstr_t>(data.value());
        }


        if(auto const data = value.calling_convention(); data.has_value())
        {
            wcout << ", CallingConvention: " << symbol_type_utils::calling_convention_to_string(data.value());
        }

        if(auto const data = value.type(); data.has_value())
        {
            wcout << ", Type: " << data.value().sym_index();
        }

        if(auto const data = value.type_id(); data.has_value())
        {
            wcout << ", TypeId: " << data.value().sym_index();
        }

        if(auto const data = value.children_count(); data.has_value())
        {
            wcout << ", ChildrenCount: " << data.value();
        }
    }

    wcout << '\n';

    if(options.debug_type_data())
    {
        if(auto const type_data = value.type(); type_data.has_value() && tag_data.value_or(dbg_help::sym_tag_enum::Null) != dbg_help::sym_tag_enum::PointerType)
        {
            dump_symbol_type(type_data.value(), options, base_offset + offset_data.value_or(0), indent + 1);
        }
    }
    else if(auto const type_data = value.type(); type_data.has_value())
    {
        if(auto const type_tag_data = type_data.value().sym_tag(); type_tag_data.value_or(dbg_help::sym_tag_enum::Null) == dbg_help::sym_tag_enum::UDT)
        {
            for (auto const& child : type_data.value().children())
            {
                dump_symbol_type(child, options, base_offset + offset_data.value_or(0), indent + 1);
            }
        }
    }

    for (auto const& child : value.children())
    {
        dump_symbol_type(child, options, base_offset, indent + 2);
    }
}

std::optional<DWORD64> get_teb_address(mini_dump const& mini_dump, thread_names_list_stream const names_list, memory_list_stream const memory_list, memory64_list_stream const memory64_list)
{
    if(auto const stream = stream_utils::find_stream_for_type(mini_dump, ThreadListStream); stream.has_value())
    {
        if(thread_list_stream const thread_list{mini_dump, std::get<size_t>(stream.value())}; thread_list.found() && thread_list.thread_list().NumberOfThreads > 0)
        {
            if(stream_thread const thread{mini_dump, thread_list.thread_list().Threads[0], names_list, memory_list, memory64_list}; thread->Teb != 0)
            {
                return thread->Teb;
            }
        }
    }

    if(auto const stream = stream_utils::find_stream_for_type(mini_dump, ThreadExListStream); stream.has_value())
    {
        if(thread_ex_list_stream const thread_ex_list{mini_dump, std::get<size_t>(stream.value())}; thread_ex_list.found() && thread_ex_list.thread_list().NumberOfThreads > 0)
        {
            if(stream_thread_ex const thread{mini_dump, thread_ex_list.thread_list().Threads[0], names_list}; thread->Teb != 0)
            {
                return thread->Teb;
            }
        }
    }

    return std::nullopt;
}

std::optional<dbg_help::symbol_type_info> get_type_info(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_type_name)
{
    auto const type_symbol_info = walker.get_type_info(symbol_type_name);
    if(!type_symbol_info.has_value())
    {
        wcout << "Failed to find symbol type: " << symbol_type_name << "\n";
    }

    return type_symbol_info;
}

std::optional<DWORD64> find_field_pointer(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& type_symbol_info, std::wstring const& symbol_type_name, DWORD64 const address, std::wstring const& field_name)
{
    auto address_value = stream_utils::find_field_pointer_value_in_type(walker, type_symbol_info, field_name, address);
    if(!address_value.has_value() || address_value.value() == 0)
    {
        wcout << "Failed to find field " << field_name << " in " << symbol_type_name << '\n';
        return std::nullopt;
    }

    return address_value.value();
}

std::optional<DWORD64> find_field_pointer(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_type_name, DWORD64 const address, std::wstring const& field_name)
{
    auto const type_symbol_info = get_type_info(walker, symbol_type_name);
    if(!type_symbol_info.has_value())
    {
        return std::nullopt;
    }

    return find_field_pointer(walker, type_symbol_info.value(), symbol_type_name, address, field_name);
}

std::optional<dbg_help::symbol_type_info> dump_field(stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_type_name, DWORD64 const address)
{
    auto const type_symbol_info = get_type_info(walker, symbol_type_name);
    if(!type_symbol_info.has_value())
    {
        return std::nullopt;
    }

    auto const data_length = type_symbol_info.value().length();
    if(!data_length.has_value() || data_length.value() == 0)
    {
        wcout << "Symbol " << symbol_type_name << " is zero length\n";
        return std::nullopt;
    }

    auto const* memory = walker.get_process_memory(address, data_length.value());
    if(memory == nullptr)
    {
        wcout << "Failed to find " << symbol_type_name << " address [" << stream_hex_dump::to_hex_full(address) << "] in dump file\n";
        return std::nullopt;
    }
    symbol_type_utils::dump_variable_symbol_at(wcout, walker, type_symbol_info.value(), type_symbol_info.value(), address, memory);
    return type_symbol_info;
}

void dump_mini_dump_peb(mini_dump const& mini_dump, [[maybe_unused]] dump_file_options const& options, [[maybe_unused]] dbg_help::symbol_engine& symbol_engine)
{
    thread_names_list_stream const names_list{mini_dump};
    memory_list_stream const memory_list{mini_dump};
    memory64_list_stream const memory64_list{mini_dump};
    function_table_stream const function_table{ mini_dump };
    module_list_stream const module_list{ mini_dump };
    unloaded_module_list_stream const unloaded_module_list{ mini_dump };
    pe_file_memory_mapping pe_file_memory_mappings{};
    stream_stack_dump::mini_dump_stack_walk const walker{
        0, nullptr, 0, memory_list, memory64_list, function_table, module_list,
        unloaded_module_list, pe_file_memory_mappings, symbol_engine
    };

    // Find TEB address
    auto const teb_address = get_teb_address(mini_dump, names_list, memory_list, memory64_list);
    if(!teb_address.has_value())
    {
        wcout << "No TEB address found\n";
        return;
    }

    auto const peb_address = find_field_pointer(walker, common_symbol_names::teb_structure_symbol_name, teb_address.value(), common_symbol_names::teb_structure_process_environment_block_field_symbol_name);
    if(!peb_address.has_value())
    {
        return;
    }

    auto peb_symbol_info = dump_field(walker, common_symbol_names::peb_structure_symbol_name, peb_address.value());
    if(!peb_symbol_info.has_value())
    {
        return;
    }

    
    if(auto const ldr_address = find_field_pointer(walker, peb_symbol_info.value(), common_symbol_names::peb_structure_symbol_name, peb_address.value(), common_symbol_names::peb_structure_ldr_field_symbol_name); ldr_address .has_value())
    {
        wcout << '\n';
        [[maybe_unused]] auto const ldr_data_symbol_info = dump_field(walker, common_symbol_names::peb_ldr_structure_symbol_name, peb_address.value());
    }

    if(auto const process_parameters_address = find_field_pointer(walker, peb_symbol_info.value(), common_symbol_names::peb_structure_symbol_name, peb_address.value(), common_symbol_names::peb_structure_process_parameters_field_symbol_name); process_parameters_address.has_value())
    {
        wcout << '\n';
        if(auto user_process_parameters_symbol_info = dump_field(walker, common_symbol_names::rtl_user_process_parameters_structure_symbol_name, peb_address.value()); user_process_parameters_symbol_info.has_value())
        {
            
            if(auto const environment_address = find_field_pointer(walker,  user_process_parameters_symbol_info.value(), common_symbol_names::rtl_user_process_parameters_structure_symbol_name, process_parameters_address.value(), common_symbol_names::rtl_user_process_parameters_structure_environment_field_symbol_name); environment_address.has_value())
            {
                DWORD64 environment_size = std::numeric_limits<uint64_t>::max();;
                auto const* environment_variable = walker.get_process_memory_range(environment_address.value(), environment_size);
                if(environment_variable == nullptr)
                {
                    wcout << "Can't find process Environment memory address [" << stream_hex_dump::to_hex_full(environment_address.value()) << "]\n";
                }

                wcout << "\nProcess Environment Variables:\n";
                print_utils::print_array_str(wcout, static_cast<wchar_t const*>(environment_variable), environment_size / sizeof(wchar_t), 2);
            }
        }
    }
}
