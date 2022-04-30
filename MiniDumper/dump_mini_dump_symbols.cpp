#include "dump_mini_dump_symbols.h"

#include <unordered_set>

#include "common_symbol_lookup_utils.h"
#include "dump_file_options.h"
#include "DbgHelpUtils/cache_manager.h"
#include "DbgHelpUtils/common_symbol_names.h"
#include "DbgHelpUtils/common_symbol_utils.h"
#include "DbgHelpUtils/print_utils.h"
#include "DbgHelpUtils/function_table_stream.h"
#include "DbgHelpUtils/gflags_utils.h"
#include "DbgHelpUtils/global_variable.h"
#include "DbgHelpUtils/global_variables.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/memory64_list_stream.h"
#include "DbgHelpUtils/memory_list_stream.h"
#include "DbgHelpUtils/mini_dump_memory_walker.h"
#include "DbgHelpUtils/module_list_stream.h"
#include "DbgHelpUtils/pe_file_memory_mapping.h"
#include "DbgHelpUtils/process_environment_block.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_utils.h"
#include "DbgHelpUtils/string_compare.h"
#include "DbgHelpUtils/symbol_type_info.h"
#include "DbgHelpUtils/symbol_type_utils.h"
#include "DbgHelpUtils/symbol_type_walker.h"
#include "DbgHelpUtils/unloaded_module_list_stream.h"

using namespace std;
using namespace dlg_help_utils;

namespace
{
    uint64_t get_stack_database_address(process::process_environment_block const& peb)
    {
        auto const symbol = peb.walker().get_symbol_info(common_symbol_names::rtl_stack_trace_database);
        if(!symbol.has_value())
        {
            return 0;
        }

        auto const address = symbol.value().address();
        if(!address.has_value())
        {
            return 0;
        }

        return stream_utils::read_machine_size_field_value(peb, address.value()).value_or(0);
    }

    void do_dump_symbol_type(std::wostream& log, dbg_help::symbol_type_info const& type, [[maybe_unused]] dump_file_options const& options, size_t const base_offset, size_t const indent, std::unordered_set<unsigned long>& visited_types)
    {
        if(visited_types.contains(type.sym_index()))
        {
            return;
        }
        visited_types.insert(type.sym_index());

        auto const tag_data = type.sym_tag();
        auto const offset_data = type.offset();
        std::wstring indent_str(indent, ' ');
        log << indent_str;

        auto const bit_position_data = type.bit_position();

        {
            std::wostringstream ss;
            if(offset_data.has_value())
            {
                ss << std::format(L"+{}", stream_hex_dump::to_hex_full(base_offset + offset_data.value()));
            }

            if(auto const str = std::move(ss).str(); !bit_position_data.has_value())
            {
                log << str;
            }
            else if(options.debug_type_data())
            {
                log << std::format(L"{} ", str);
            }
            else
            {
                log << std::wstring(str.size() + 1, ' ');
            }
        }

        auto name = symbol_type_utils::get_symbol_type_friendly_name(type);
        log << std::format(L" {}", name);

        if(bit_position_data.has_value())
        {
            auto const length_data = type.length();
            uint64_t bit_mask;
            if(length_data.value_or(1) > 1)
            {
                bit_mask = (~(std::numeric_limits<uint64_t>::max() << length_data.value())) << bit_position_data.value();
                log << std::format(L" bits {0} - {1} ({2})"
                    , locale_formatting::to_wstring(bit_position_data.value())
                    , locale_formatting::to_wstring(bit_position_data.value() + length_data.value() - 1)
                    , stream_hex_dump::to_hex(bit_mask));
            }
            else
            {
                bit_mask = 0x01ULL << bit_position_data.value();
                log << std::format(L" bits {0} ({1})"
                    , locale_formatting::to_wstring(bit_position_data.value())
                    , stream_hex_dump::to_hex(bit_mask));
            }

        }
        else
        {
            if(auto const data = type.length(); data.has_value())
            {
                log << std::format(L", Length: {}", locale_formatting::to_wstring(data.value()));
            }
        }

        if(options.debug_type_data())
        {
            if(bit_position_data.has_value())
            {
                if(auto const data = type.length(); data.has_value())
                {
                    log << std::format(L", Length: {}", locale_formatting::to_wstring(data.value()));
                }
            }

            log << std::format(L", SymIndex: {}", locale_formatting::to_wstring(type.sym_index()));

            if(tag_data.has_value())
            {
                log << std::format(L", tag: {}", symbol_type_utils::sym_tag_to_string(tag_data.value()));
            }

            if(auto const data = type.base_type(); data.has_value())
            {
                log << std::format(L", BaseType: {}",  symbol_type_utils::basic_type_to_string(data.value()));
            }
            
            if(auto const data = type.data_kind(); data.has_value())
            {
                log << std::format(L", DataKind: {}", symbol_type_utils::data_kind_convention_to_string(data.value()));
            }

            if(auto const data = type.address_offset(); data.has_value())
            {
                log << std::format(L", AddressOffset: {}", locale_formatting::to_wstring(data.value()));
            }

            if(auto const data = type.array_index_typeid(); data.has_value())
            {
                log << std::format(L", ArrayIndexTypeId: {}", locale_formatting::to_wstring(data.value()));
            }

            if(auto const data = type.array_count(); data.has_value())
            {
                log << std::format(L", ArrayCount: {}", locale_formatting::to_wstring(data.value()));
            }

            if(auto const data = type.bit_position(); data.has_value())
            {
                log << std::format(L", BitPosition: {}", locale_formatting::to_wstring(data.value()));
            }

            if(auto const data = type.is_virtual_base_class(); data.has_value())
            {
                log << std::format(L", IsVirtualBaseClass: {}", data.value());
            }

            if(auto const data = type.virtual_base_pointer_offset(); data.has_value())
            {
                log << std::format(L", VirtualBasePointerOffset: {}", locale_formatting::to_wstring(data.value()));
            }

            if(auto const data = type.class_parent_id(); data.has_value())
            {
                log << std::format(L", ClassParentId: {}", locale_formatting::to_wstring(data.value().sym_index()));
            }

            if(auto const data = type.nested(); data.has_value())
            {
                log << std::format(L", Nested: {}", locale_formatting::to_wstring(data.value()));
            }

            if(auto const data = type.lexical_parent(); data.has_value())
            {
                log << std::format(L", LexicalParent: {}", locale_formatting::to_wstring(data.value().sym_index()));
            }

            if(auto const data = type.address(); data.has_value())
            {
                log << std::format(L", Address: {}", locale_formatting::to_wstring(data.value()));
            }

            if(auto const data = type.virtual_base_offset(); data.has_value())
            {
                log << std::format(L", VirtualBaseOffset: {}", locale_formatting::to_wstring(data.value()));
            }

            if(auto const data = type.virtual_base_displacement_table_index(); data.has_value())
            {
                log << std::format(L", VirtualBaseDisplacementTableIndex: {}", locale_formatting::to_wstring(data.value()));
            }

            if(auto const data = type.is_reference(); data.has_value())
            {
                log << std::format(L", IsReference: {}", data.value());
            }

            if(auto const data = type.indirect_virtual_base_class(); data.has_value())
            {
                log << std::format(L", IndirectVirtualBaseClass: {}", data.value());
            }

            if(auto data = type.const_value(); V_VT(&data) != VT_EMPTY)
            {
                _variant_t vt;
                vt.Attach(data);
                auto const string_const_value = static_cast<_bstr_t>(data);
                log << std::format(L", ConstValue: {}", static_cast<wchar_t const*>(string_const_value));
            }

            if(auto const data = type.calling_convention(); data.has_value())
            {
                log << std::format(L", CallingConvention: {}", symbol_type_utils::calling_convention_to_string(data.value()));
            }

            if(auto const data = type.type(); data.has_value())
            {
                log << std::format(L", Type: {}", locale_formatting::to_wstring(data.value().sym_index()));
            }

            if(auto const data = type.type_id(); data.has_value())
            {
                log << std::format(L", TypeId: {}", locale_formatting::to_wstring(data.value().sym_index()));
            }

            if(auto const data = type.children_count(); data.has_value())
            {
                log << std::format(L", ChildrenCount: {}", locale_formatting::to_wstring(data.value()));
            }
        }

        log << L'\n';

        if(options.debug_type_data())
        {
            if(auto const type_data = type.type(); type_data.has_value() && !visited_types.contains(type_data.value().sym_index()))
            {
                log << std::format(L"{0} Type: {1}\n", indent_str, locale_formatting::to_wstring(type_data.value().sym_index()));
                do_dump_symbol_type(log, type_data.value(), options, base_offset + offset_data.value_or(0), indent + 1, visited_types);
            }

            if(auto const typeid_data = type.type_id(); typeid_data.has_value() && !visited_types.contains(typeid_data.value().sym_index()))
            {
                log << std::format(L"{0} TypeId: {1}\n", indent_str, locale_formatting::to_wstring(typeid_data.value().sym_index()));
                do_dump_symbol_type(log, typeid_data.value(), options, base_offset + offset_data.value_or(0), indent + 1, visited_types);
            }

            if(options.debug_type_parent_data())
            {
                if(auto const parent_typeid_data = type.class_parent_id(); parent_typeid_data.has_value() && !visited_types.contains(parent_typeid_data.value().sym_index()))
                {
                    log << std::format(L"{0} ClassParentId: {1}\n", indent_str, locale_formatting::to_wstring(parent_typeid_data.value().sym_index()));
                    do_dump_symbol_type(log, parent_typeid_data.value(), options, base_offset + offset_data.value_or(0), indent + 1, visited_types);
                }

                if(auto const parent_id_data = type.lexical_parent(); parent_id_data.has_value() && !visited_types.contains(parent_id_data.value().sym_index()))
                {
                    log << std::format(L"{0} LexicalParentId: {1}\n", indent_str, locale_formatting::to_wstring(parent_id_data.value().sym_index()));
                    do_dump_symbol_type(log, parent_id_data.value(), options, base_offset + offset_data.value_or(0), indent + 1, visited_types);
                }
            }

            if(auto const data = type.children_count(); data.value_or(0) > 0)
            {
                log << std::format(L"{0} Children: {1}\n", indent_str, locale_formatting::to_wstring(data.value()));
            }
        }
        else if(auto const type_data = type.type(); type_data.has_value())
        {
            if(auto const type_tag_data = type_data.value().sym_tag(); type_tag_data.value_or(dbg_help::sym_tag_enum::Null) == dbg_help::sym_tag_enum::UDT)
            {
                for (auto const& child : type_data.value().children())
                {
                    do_dump_symbol_type(log, child, options, base_offset + offset_data.value_or(0), indent + 1, visited_types);
                }
            }
        }

        for (auto const& child : type.children())
        {
            do_dump_symbol_type(log, child, options, base_offset, indent + 2, visited_types);
        }
    }
}

void dump_mini_dump_symbol_type(std::wostream& log, mini_dump const& mini_dump, std::wstring const& type_name, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    memory_list_stream const memory_list{mini_dump};
    memory64_list_stream const memory64_list{ mini_dump };
    function_table_stream const function_table{ mini_dump };
    module_list_stream const module_list{ mini_dump };
    unloaded_module_list_stream const unloaded_module_list{ mini_dump };
    pe_file_memory_mapping pe_file_memory_mappings{};
    stream_stack_dump::mini_dump_memory_walker const walker{
        0, nullptr, 0, memory_list, memory64_list, function_table, module_list,
        unloaded_module_list, pe_file_memory_mappings, symbol_engine
    };

    if(auto const symbol_info = dbg_help::symbol_type_info::from_address_string(symbol_engine.process(), type_name); symbol_info.has_value())
    {
        if(walker.load_module_from_address(symbol_info.value().module_base()))
        {
            log << std::format(L"Symbol Type Address {}:\n", type_name);
            dump_symbol_type(log, symbol_info.value(), options);
        }
        else
        {
            log << std::format(L"No module found for Symbol Type Address {}:\n", type_name);
        }
        return;
    }

    if(auto const symbol_info = walker.get_type_info(type_name); symbol_info.has_value())
    {
        log << std::format(L"Symbol Type [{0}] @ {1} found:\n", type_name, symbol_info.value().to_address_string());
        dump_symbol_type(log, symbol_info.value(), options);
        return;
    }

    if(stream_stack_dump::symbol_type_walker const symbol_walker{walker, type_name}; !symbol_walker.all_symbols().empty())
    {
        for(auto const& symbol_info : symbol_walker.all_symbols())
        {
            log << std::format(L"Symbol Type [{0}] @ {1} found:\n", symbol_info.name().value_or(L"<unknown name>"sv), symbol_info.to_address_string());
            dump_symbol_type(log, symbol_info, options);
        }
        return;
    }

    log << std::format(L"Symbol Type [{}] not found\n", type_name);
}

void dump_mini_dump_symbol_name(std::wostream& log, mini_dump const& mini_dump, std::wstring const& symbol_name, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    memory_list_stream const memory_list{mini_dump};
    memory64_list_stream const memory64_list{ mini_dump };
    function_table_stream const function_table{ mini_dump };
    module_list_stream const module_list{ mini_dump };
    unloaded_module_list_stream const unloaded_module_list{ mini_dump };
    pe_file_memory_mapping pe_file_memory_mappings{};
    stream_stack_dump::mini_dump_memory_walker const walker{
        0, nullptr, 0, memory_list, memory64_list, function_table, module_list,
        unloaded_module_list, pe_file_memory_mappings, symbol_engine
    };

    auto processed_any{false};
    for (process::global_variables variables{walker, symbol_name};
        auto const& variable : variables.all_variables())
    {
        processed_any = true;
        auto const name = symbol_type_utils::get_symbol_type_friendly_name(variable.symbol_type());
        log << std::format(L"Symbol Name [{}] found:\n", name);

        if(options.debug_type_data())
        {
            dump_symbol_type(log, variable.symbol_type(), options);
        }

        auto const address = variable.symbol_type().address();
        if(auto const type = variable.symbol_type().type(); type.has_value() && address.has_value())
        {
            symbol_type_utils::dump_variable_type_at(log, mini_dump, symbol_engine, type.value(), address.value());
        }
        else if(auto stream = variable.stream(); !stream.eof())
        {
            hex_dump::hex_dump(log, stream, stream.length());
        }
        else if(address.has_value())
        {
            log << std::format(L"Can't find memory address [{}]\n", stream_hex_dump::to_hex_full(address.value()));
        }
    }

    if(!processed_any)
    {
        log << std::format(L"Symbol Name [{}] not found\n", symbol_name);
    }
}

void dump_mini_dump_module_symbol_types(std::wostream& log, mini_dump const& mini_dump, std::wstring const& module_name, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    memory_list_stream const memory_list{mini_dump};
    memory64_list_stream const memory64_list{ mini_dump };
    function_table_stream const function_table{ mini_dump };
    module_list_stream const module_list{ mini_dump };
    unloaded_module_list_stream const unloaded_module_list{ mini_dump };
    pe_file_memory_mapping pe_file_memory_mappings{};
    stream_stack_dump::mini_dump_memory_walker const walker{
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
            log << L'\n';
        }

        dump_symbol_type(log, type, options);
    }
}

template<typename T>
void dump_address_type_array(std::wostream& log, std::wstring const& dt, mini_dump_memory_stream& variable_stream, uint64_t const memory_size, size_t const elements_per_line, size_t const element_width, size_t const indent, bool const dump_hex)
{
    log << std::format(L"{0} array @ [{1}] for [{2}] elements:\n"
        , dt
        , stream_hex_dump::to_hex_full(variable_stream.current_address())
        , locale_formatting::to_wstring(memory_size / sizeof(T)));
    print_utils::print_stream_array_lines<T>(log, variable_stream, memory_size / sizeof(T), elements_per_line, element_width, indent, dump_hex);
    log << L'\n';
}

void dump_mini_dump_address(std::wostream& log, mini_dump const& mini_dump, std::wstring const& address, [[maybe_unused]] dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    memory_list_stream const memory_list{mini_dump};
    memory64_list_stream const memory64_list{ mini_dump };
    function_table_stream const function_table{ mini_dump };
    module_list_stream const module_list{ mini_dump };
    unloaded_module_list_stream const unloaded_module_list{ mini_dump };
    pe_file_memory_mapping pe_file_memory_mappings{};
    stream_stack_dump::mini_dump_memory_walker const walker{
        0, nullptr, 0, memory_list, memory64_list, function_table, module_list,
        unloaded_module_list, pe_file_memory_mappings, symbol_engine
    };

    if(auto [memory_pointer, symbol_type, memory_size, dt] = symbol_type_utils::parse_address(address); !symbol_type.empty())
    {
        symbol_type_utils::dump_variable_type_at(log, mini_dump, symbol_engine, symbol_type, memory_pointer);
    }
    else
    {
        auto const find_limit = memory_size == 0;
        if(find_limit)
        {
            memory_size = std::numeric_limits<uint64_t>::max();
        }

        auto stream = walker.get_process_memory_stream(memory_pointer, memory_size);
        if(stream.eof())
        {
            log << std::format(L"Can't find memory address [{}]\n", stream_hex_dump::to_hex_full(memory_pointer));
        }

        if(dt.empty())
        {
            hex_dump::hex_dump(log, stream, memory_size);
        }
        else if(string_compare::iequals(dt, L"str"sv))
        {
            log << std::format(L"string @ [{}]:\n", stream_hex_dump::to_hex_full(memory_pointer));
            print_utils::print_stream_str<char>(log, stream, memory_size, find_limit);
            log << L'\n';
        }
        else if(string_compare::iequals(dt, L"wstr"sv))
        {
            log << std::format(L"unicode string @ [{}]:\n", stream_hex_dump::to_hex_full(memory_pointer));
            print_utils::print_stream_str<wchar_t>(log, stream, memory_size / sizeof(wchar_t), find_limit);
            log << L'\n';
        }
        else if(string_compare::iequals(dt, L"astr"sv))
        {
            log << std::format(L"string array @ [{}]:\n", stream_hex_dump::to_hex_full(memory_pointer));
            print_utils::print_stream_array_str<char>(log, stream, memory_size, 2);
            log << L'\n';
        }
        else if(string_compare::iequals(dt, L"awstr"sv))
        {
            log << std::format(L"unicode string array @ [{}]:\n", stream_hex_dump::to_hex_full(memory_pointer));
            print_utils::print_stream_array_str<wchar_t>(log, stream, memory_size / sizeof(wchar_t), 2);
            log << L'\n';
        }
        else
        {
            auto display_hex = false;
            if(dt.ends_with(L",h"s) || dt.ends_with(L",H"s))
            {
                dt = dt.substr(0, dt.size() - 2);
                display_hex = true;
            }
            if(string_compare::iequals(dt, L"uint8"sv))
            {
                dump_address_type_array<uint8_t>(log, dt, stream, memory_size, 16, 6, 2, display_hex);
            }
            else if(string_compare::iequals(dt, L"int8"sv))
            {
                dump_address_type_array<int8_t>(log, dt, stream, memory_size, 16, 6, 2, display_hex);
            }
            else if(string_compare::iequals(dt, L"uint16"sv))
            {
                dump_address_type_array<uint16_t>(log, dt, stream, memory_size, 8, 8, 2, display_hex);
            }
            else if(string_compare::iequals(dt, L"int16"sv))
            {
                dump_address_type_array<int16_t>(log, dt, stream, memory_size, 8, 8, 2, display_hex);
            }
            else if(string_compare::iequals(dt, L"uint32"sv))
            {
                dump_address_type_array<uint32_t>(log, dt, stream, memory_size, 4, 14, 2, display_hex);
            }
            else if(string_compare::iequals(dt, L"int32"sv))
            {
                dump_address_type_array<int32_t>(log, dt, stream, memory_size, 4, 14, 2, display_hex);
            }
            else if(string_compare::iequals(dt, L"uint64"sv))
            {
                dump_address_type_array<uint64_t>(log, dt, stream, memory_size, 2, 24, 2, display_hex);
            }
            else if(string_compare::iequals(dt, L"int64"sv))
            {
                dump_address_type_array<int64_t>(log, dt, stream, memory_size, 2, 24, 2, display_hex);
            }
            else if(string_compare::iequals(dt, L"float"sv))
            {
                dump_address_type_array<float>(log, dt, stream, memory_size, 2, 24, 2, display_hex);
            }
            else if(string_compare::iequals(dt, L"double"sv))
            {
                dump_address_type_array<double>(log, dt, stream, memory_size, 2, 24, 2, display_hex);
            }
            else
            {
                log << std::format(L"Invalid dt [{}]\n", dt);
            }
        }
    }
}


void dump_symbol_type(std::wostream& log, dbg_help::symbol_type_info const& type, [[maybe_unused]] dump_file_options const& options, size_t const base_offset, size_t const indent)
{
    std::unordered_set<unsigned long> visited_types;
    do_dump_symbol_type(log, type, options, base_offset, indent, visited_types);
}

void dump_mini_dump_peb(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, [[maybe_unused]] dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, cache, symbol_engine};

    [[maybe_unused]] auto const peb_symbol_info = dump_field(log, peb.walker(), common_symbol_names::peb_structure_symbol_name, peb.peb_address());

    log << L'\n';
    const auto values = dump_gflags_to_strings(peb.nt_global_flag());
    log << std::format(L"NtGlobalFlag: ({})\n", stream_hex_dump::to_hex_full(static_cast<uint32_t>(peb.nt_global_flag())));
    for (auto const& value : values)
    {
        log << std::format(L"  {}\n", value);
    }

    if(auto const ldr_address = peb.ldr_address(); ldr_address != 0)
    {
        log << L'\n';
        [[maybe_unused]] auto const ldr_data_symbol_info = dump_field(log, peb.walker(), common_symbol_names::peb_ldr_structure_symbol_name, ldr_address);
    }

    if(auto const environment_variables = peb.process_environment_variables(); environment_variables.has_value())
    {
        log << L'\n';
        [[maybe_unused]] const auto user_process_parameters_symbol_info = dump_field(log, peb.walker(), common_symbol_names::rtl_user_process_parameters_structure_symbol_name, environment_variables.value().process_parameters_address());

        log << L"\nProcess Environment Variables:\n";
        for(auto const& value : environment_variables.value().environment())
        {
            log << L"  ";
            print_utils::print_str(log, value.data(), value.size(), false);
            log << L'\n';
        }
    }
}

void dump_mini_dump_stack_trace_database(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, [[maybe_unused]] dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, cache, symbol_engine};

    auto const stack_database_address = get_stack_database_address(peb);
    if(stack_database_address == 0)
    {
        log << L"No Stack Database in DMP\n";
    }

    [[maybe_unused]] auto const stack_database_symbol_info = dump_field(log, peb.walker(), common_symbol_names::stack_trace_database_structure_symbol_name, stack_database_address);
}
