#include "symbol_type_utils.h"

#include <format>
#include <unordered_map>

#include "print_utils.h"
#include "function_table_stream.h"
#include "locale_number_formatting.h"
#include "memory64_list_stream.h"
#include "memory_list_stream.h"
#include "mini_dump_memory_walker.h"
#include "module_list_stream.h"
#include "pe_file_memory_mapping.h"
#include "size_units.h"
#include "stream_hex_dump.h"
#include "string_utils.h"
#include "symbol_data_dumper.h"
#include "symbol_type_info.h"
#include "unloaded_module_list_stream.h"

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace dlg_help_utils::dbg_help;

namespace dlg_help_utils::symbol_type_utils
{
    namespace
    {
        struct symbol_walker_work
        {
            symbol_type_info type;
            uint64_t variable_address;
        };

        void do_gather_all_pointers_from_symbol(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& base_type
            , symbol_type_info const& type
            , uint64_t base_address
            , uint64_t variable_address_offset
            , std::map<uint64_t, pointer_info>& pointers
            , std::wstring const& name_prefix
            , std::unordered_set<uint64_t> const& ignore_pointers
            , std::map<uint64_t, symbol_walker_work>& work);

        template<typename T>
        void read_possible_pointer_type(stream_stack_dump::mini_dump_memory_walker const& walker
            , mini_dump_memory_stream& variable_stream
            , std::optional<symbol_type_info> const& pointer_type
            , pointer_info info
            , std::map<uint64_t, pointer_info>& pointers
            , std::unordered_set<uint64_t> const& ignore_pointers
            , std::map<uint64_t, symbol_walker_work>& work)
        {
            if(T value{}; variable_stream.read(&value, sizeof value) == sizeof value)
            {
                if(value > 0 
                    && !ignore_pointers.contains(value) 
                    && !pointers.contains(value)
                    && !work.contains(value)
                    && walker.find_memory_range(value, 1, 1) == 1)
                {
                    info.pointer_address = value;
                    pointers.insert(std::make_pair(value, info));
                    if(pointer_type.has_value())
                    {
                        work.insert(std::make_pair(value, symbol_walker_work{pointer_type.value(), value}));
                    }
                }
            }
        }

        pointer_info make_pointer_info(std::wstring name, uint64_t const base_address, uint64_t const variable_address_offset, symbol_type_info base_type, uint64_t const pointer, symbol_type_info type)
        {
            return
            {
                .name= std::move(name),
                .base_address= base_address,
                .base_variable_address_offset= variable_address_offset,
                .base_variable_type= std::move(base_type),
                .pointer_address= pointer,
                .pointer_type= std::move(type)
            };
        }

        std::wstring_view get_symbol_name(symbol_type_info const& value)
        {
            auto const name_data = value.name();
            return name_data.value_or(resources::get_symbol_name_none());
        }

        std::wstring generate_name_prefix(std::wstring const& name_prefix, symbol_type_info const& type)
        {
            if(name_prefix.empty())
            {
                return get_symbol_type_friendly_name(type);
            }

            return std::format(L"{0}.{1}", name_prefix, get_symbol_type_friendly_name(type));
        }

        uint64_t memory_address_from_string(std::wstring const& value)
        {
            size_t index{0};
            return std::stoull(value, &index, 0);
        }

        void do_gather_all_pointers_from_symbol(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& base_type
            , symbol_type_info const& type
            , uint64_t const base_address
            , uint64_t const variable_address_offset
            , std::map<uint64_t, pointer_info>& pointers
            , std::wstring const& name_prefix
            , std::unordered_set<uint64_t> const& ignore_pointers
            , std::map<uint64_t, symbol_walker_work>& work)
        {
            switch(auto const tag = type.sym_tag().value_or(sym_tag_enum::Null); tag)  // NOLINT(clang-diagnostic-switch-enum)
            {
            case sym_tag_enum::UDT:
            case sym_tag_enum::Data:
                if(auto const data_type = type.type(); data_type.has_value())
                {
                    do_gather_all_pointers_from_symbol(walker, base_type, data_type.value(), base_address, variable_address_offset, pointers, generate_name_prefix(name_prefix, type), ignore_pointers, work);
                }
                break;

            case sym_tag_enum::ArrayType:
                if(auto const data_type = type.type(); data_type.has_value())
                {
                    auto const data_type_length = data_type.value().length();
                    auto const data_type_tag = data_type.value().sym_tag();
                    // ReSharper disable once CppTooWideScopeInitStatement
                    auto const array_size = type.array_count().value_or(0);

                    if(data_type_length.has_value() && data_type_tag.has_value() && array_size > 0)
                    {
                        auto variable_array_entry_address = variable_address_offset;
                        for(DWORD i = 0; i < array_size; ++i, variable_array_entry_address += data_type_length.value())
                        {
                            do_gather_all_pointers_from_symbol(walker, base_type, data_type.value(), base_address, variable_array_entry_address, pointers, std::format(L"{0} @ [{1}]", name_prefix, i), ignore_pointers, work);
                        }
                    }
                }
                break;

            case sym_tag_enum::PointerType:
                if(auto const type_length = type.length(); type_length.has_value())
                {
                    auto const length = type.length();
                    if(!length.has_value())
                    {
                        return;
                    }

                    auto variable_stream = walker.get_process_memory_stream(base_address + variable_address_offset, length.value());
                    if(variable_stream.eof())
                    {
                        return;
                    }

                    switch(type_length.value())
                    {
                    case 4:
                        read_possible_pointer_type<uint32_t>(walker
                            , variable_stream
                            , type.type()
                            , make_pointer_info(name_prefix, base_address, variable_address_offset, base_type, base_address + variable_address_offset, type.type().value_or(type))
                            , pointers
                            , ignore_pointers
                            , work);
                        break;

                    case 8:
                        read_possible_pointer_type<uint64_t>(walker
                            , variable_stream
                            , type.type()
                            , make_pointer_info(name_prefix, base_address, variable_address_offset, base_type, base_address + variable_address_offset, type.type().value_or(type))
                            , pointers
                            , ignore_pointers
                            , work);
                        break;

                    default:
                        break;
                    }
                }
                break;

            default:
                break;
            }

            for (auto const sub_name_prefix{generate_name_prefix(name_prefix, type)}; auto const& child : type.children())
            {
                if(auto const offset_data = child.offset(); offset_data.has_value())
                {
                    if(auto const tag = child.sym_tag().value_or(sym_tag_enum::Null); symbol_data_dumper::can_dump_tag(tag))
                    {
                        do_gather_all_pointers_from_symbol(walker, base_type, child, base_address + variable_address_offset, offset_data.value(), pointers, sub_name_prefix, ignore_pointers, work);
                    }
                }
            }
        }

        void dump_render_line(std::wostream& os, size_t const indent, dump_variable_symbol_data const& data)
        {
            auto line = data.render_line();
            string_utils::indent_string(line, indent);
            os << line << '\n';
        }

        void dump_sub_lines(std::wostream& os, std::function<generator<dump_variable_symbol_data>()> const& sub_lines, size_t const indent)
        {
            if (!sub_lines)
            {
                return;
            }

            for (const auto& data : sub_lines())
            {
                dump_render_line(os, indent, data);
                dump_sub_lines(os, data.sub_lines, indent + 1);
            }
        }
    }

    std::wstring get_symbol_type_friendly_name(symbol_type_info const& value)
    {
        std::wstring name;
        if (auto const tag_data = value.sym_tag(); tag_data.has_value())
        {
            switch (tag_data.value())  // NOLINT(clang-diagnostic-switch-enum)
            {
            case sym_tag_enum::UDT:
            {
                if (auto const kind_data = value.udt_kind(); kind_data.has_value())
                {
                    name = resources::udt_kind_to_string(kind_data.value());
                }

                if(!name.empty())
                {
                    name += L' ';
                }

                name += get_symbol_name(value);
                break;
            }

            case sym_tag_enum::Enum:
                name = std::format(L"{} {}", resources::get_enum_title(), get_symbol_name(value));
                break;

            case sym_tag_enum::PointerType:
                if (auto const type_data = value.type(); type_data.has_value())
                {
                    name = get_symbol_type_friendly_name(type_data.value());
                    if (auto const reference_data = value.is_reference(); reference_data.has_value() && reference_data.value())
                    {
                        name += resources::get_reference_title();
                    }
                    else
                    {
                        name += resources::get_pointer_title();
                    }
                }
                break;

            case sym_tag_enum::ArrayType:
                if (auto const type_data = value.type(); type_data.has_value())
                {
                    name = get_symbol_type_friendly_name(type_data.value());
                }
                if (auto const data_array_count = value.array_count(); data_array_count.has_value())
                {
                    name += L"["s + std::to_wstring(data_array_count.value()) + L"]"s;
                }
                break;

            case sym_tag_enum::BaseType:
            {
                if (auto const data_base_type = value.base_type(); data_base_type.has_value())
                {
                    auto do_type = true;
                    if (data_base_type.value() == basic_type::Int || data_base_type.value() == basic_type::Long)
                    {
                        if (auto const data_length = value.length(); data_length.has_value())
                        {
                            switch (data_length.value())
                            {
                            case 1:
                                name = resources::get_int8_title();
                                do_type = false;
                                break;

                            case 2:
                                name = resources::get_int16_title();
                                do_type = false;
                                break;

                            case 4:
                                name = resources::get_int32_title();
                                do_type = false;
                                break;

                            case 8:
                                name = resources::get_int64_title();
                                do_type = false;
                                break;

                            case 16:
                                name = resources::get_int128_title();
                                do_type = false;
                                break;

                            case 32:
                                name = resources::get_int256_title();
                                do_type = false;
                                break;

                            default:
                                break;
                            }
                        }
                    }
                    else if (data_base_type.value() == basic_type::UInt || data_base_type.value() == basic_type::ULong)
                    {
                        if (auto const data_length = value.length(); data_length.has_value())
                        {
                            switch (data_length.value())
                            {
                            case 1:
                                name = resources::get_uint8_title();
                                do_type = false;
                                break;

                            case 2:
                                name = resources::get_uint16_title();
                                do_type = false;
                                break;

                            case 4:
                                name = resources::get_uint32_title();
                                do_type = false;
                                break;

                            case 8:
                                name = resources::get_uint64_title();
                                do_type = false;
                                break;

                            case 16:
                                name = resources::get_uint128_title();
                                do_type = false;
                                break;

                            case 32:
                                name = resources::get_uint256_title();
                                do_type = false;
                                break;

                            default:
                                break;
                            }
                        }
                    }
                    else if (data_base_type.value() == basic_type::Float)
                    {
                        if (auto const data_length = value.length(); data_length.has_value())
                        {
                            switch (data_length.value())
                            {
                            case 4:
                                name = resources::get_float32_title();
                                do_type = false;
                                break;

                            case 8:
                                name = resources::get_float64_title();
                                do_type = false;
                                break;

                            case 16:
                                name = resources::get_float128_title();
                                do_type = false;
                                break;

                            case 32:
                                name = resources::get_float256_title();
                                do_type = false;
                                break;

                            default:
                                break;
                            }
                        }
                    }

                    if (do_type)
                    {
                        name = resources::basic_type_to_string(data_base_type.value());
                    }
                }
                break;
            }

            case sym_tag_enum::Data:
                if (auto const type_id_data = value.type_id(); type_id_data.has_value())
                {
                    name = get_symbol_type_friendly_name(type_id_data.value());
                }

                if (auto const name_data = value.name(); name_data.has_value())
                {
                    if (!name.empty())
                    {
                        name += L' ';
                    }
                    name += name_data.value();
                }
                break;

            case sym_tag_enum::FunctionType:
                if (auto const func_type_data = value.type(); func_type_data.has_value())
                {
                    name = get_symbol_type_friendly_name(func_type_data.value());
                    if(auto const calling_convention = value.calling_convention(); calling_convention.has_value())
                    {
                        if (!name.empty())
                        {
                            name += L' ';
                        }
                        name += resources::calling_convention_to_string(calling_convention.value());
                    }

                    if (auto const name_data = value.name(); name_data.has_value())
                    {
                        if (!name.empty())
                        {
                            name += L' ';
                        }
                        name += name_data.value();
                    }

                    name += L'(';
                    for (auto first = true; auto const& arg_type : func_type_data.value().children())
                    {
                        if (first)
                        {
                            first = false;
                        }
                        else
                        {
                            name += L", "s;
                        }
                        name += get_symbol_type_friendly_name(arg_type);
                    }
                    name += L')';
                }
                break;

            case sym_tag_enum::FunctionArgType:
                if (auto const type_id_data = value.type_id(); type_id_data.has_value())
                {
                    name = get_symbol_type_friendly_name(type_id_data.value());
                }

                if (auto const name_data = value.name(); name_data.has_value())
                {
                    if (!name.empty())
                    {
                        name += L' ';
                    }
                    name += name_data.value();
                }
                break;

            case sym_tag_enum::Typedef:
                name = get_symbol_name(value);
                break;

            default: break;
            }

            if (name.empty())
            {
                name = resources::sym_tag_to_string(tag_data.value());
                if (auto const name_data = value.name(); name_data.has_value())
                {
                    if (!name.empty())
                    {
                        name += ' ';
                    }
                    name += name_data.value();
                }
            }
        }

        return name;
    }

    void dump_variable_type_at(
        std::wostream& os
        , mini_dump const& mini_dump
        , symbol_visit_flags::flags const options
        , size_t const max_symbol_dump_depth
        , symbol_data_dumper const& symbol_data_dumper
        , symbol_engine& symbol_engine
        , std::wstring const& symbol_type_name
        , uint64_t const variable_address
        , size_t const indent)
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

        auto const symbol_info = walker.get_type_info(symbol_type_name);
        if(!symbol_info.has_value())
        {
            os << std::format(L"{} [{}]\n", resources::get_failed_to_find_string(), symbol_type_name);
            return;
        }

        auto const length = symbol_info.value().length();
        if(!length.has_value())
        {
            os << std::format(L"{} [{}] {}\n", resources::get_symbol_title(), symbol_type_name, resources::get_is_zero_length_string());
            return;
        }

        auto stream = walker.get_process_memory_stream(variable_address, length.value());
        if(stream.eof())
        {
            os << std::format(L"{} [{} - {}] {}\n", resources::get_failed_to_find_string(), stream_hex_dump::to_hex(variable_address), size_units::base_16::to_wstring(size_units::base_16::bytes{length.value()}), resources::get_memory_address_in_range_string());
            return;
        }

        dump_variable_symbol_at(os, walker, options, max_symbol_dump_depth, symbol_data_dumper, symbol_info.value(), symbol_info.value(), symbol_type_name, variable_address, stream, indent);
    }

    void dump_variable_type_at(
        std::wostream& os
        , mini_dump const& mini_dump
        , symbol_visit_flags::flags const options
        , size_t const max_symbol_dump_depth
        , symbol_data_dumper const& symbol_data_dumper
        , symbol_engine& symbol_engine
        , symbol_type_info const& symbol_info
        , std::wstring const& symbol_type_name
        , uint64_t const variable_address
        , size_t const indent)
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

        auto const length = symbol_info.length();
        if(!length.has_value())
        {
            os << std::format(L"{} [{} - {}] {}\n", resources::get_symbol_title(), symbol_type_name, get_symbol_type_friendly_name(symbol_info), resources::get_is_zero_length_string());
            return;
        }

        auto stream = walker.get_process_memory_stream(variable_address, length.value());
        if(stream.eof())
        {
            os << std::format(L"{} {} [{} - {}] {}\n", resources::get_failed_to_find_string(), symbol_type_name, stream_hex_dump::to_hex(variable_address), size_units::base_16::to_wstring(size_units::base_16::bytes{length.value()}), resources::get_memory_address_in_range_string());
            return;
        }

        dump_variable_symbol_at(os, walker, options, max_symbol_dump_depth, symbol_data_dumper, symbol_info, symbol_info, symbol_type_name, variable_address, stream, indent);
    }

    void dump_variable_symbol_at(
        std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , size_t const max_symbol_dump_depth
        , symbol_data_dumper const& symbol_data_dumper
        , symbol_type_info const& type
        , symbol_type_info const& display_type
        , std::wstring const& symbol_type_name
        , uint64_t const variable_address
        , mini_dump_memory_stream const& variable_stream
        , size_t const indent)
    {
        std::unordered_set visited_pointers{ variable_address };
        auto data = symbol_data_dumper.variable_symbol_at(
            walker
            , options
            , {}
            , type
            , display_type
            , variable_address
            , variable_stream
            , is_head_t{true}
            , symbol_type_name
            , symbol_type_name
            , visited_pointers
            , max_symbol_dump_depth);

        dump_render_line(os, indent, data);
        dump_sub_lines(os, data.sub_lines, indent + 1);
    }

    std::tuple<uint64_t, std::wstring, uint64_t, std::wstring> parse_address(std::wstring const& address)
    {
        auto pos = address.find_first_of(L'$');
        if(pos != std::wstring::npos)
        {
            return std::make_tuple(memory_address_from_string(address.substr(0, pos)), address.substr(pos + 1), uint64_t{0}, std::wstring{});
        }

        pos = address.find_first_of(L'-');
        if(pos != std::wstring::npos)
        {
            if(auto const dt_pos = address.find_first_of(L'!', pos + 1); dt_pos != std::wstring::npos)
            {
                return std::make_tuple(memory_address_from_string(address.substr(0, pos)), std::wstring{}, memory_address_from_string(address.substr(pos + 1, dt_pos - (pos + 1))), address.substr(dt_pos + 1));
            }

            return std::make_tuple(memory_address_from_string(address.substr(0, pos)), std::wstring{}, memory_address_from_string(address.substr(pos + 1)), std::wstring{});
        }

        pos = address.find_first_of(L'!');
        if(pos != std::wstring::npos)
        {
            return std::make_tuple(memory_address_from_string(address.substr(0, pos)), std::wstring{}, 0, address.substr(pos + 1));
        }

        return std::make_tuple(memory_address_from_string(address), std::wstring{}, uint64_t{0}, std::wstring{});
    }

    void gather_all_pointers_from_symbol(stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_type_info const& base_type
        , symbol_type_info const& type
        , uint64_t const base_address
        , uint64_t const variable_address_offset
        , std::map<uint64_t, pointer_info>& pointers
        , std::wstring const& name_prefix
        , std::unordered_set<uint64_t> const& ignore_pointers)
    {
        std::map<uint64_t, symbol_walker_work> work;
        do_gather_all_pointers_from_symbol(walker
            , base_type
            , type
            , base_address
            , variable_address_offset
            , pointers
            , name_prefix
            , ignore_pointers
            , work);

        while(!work.empty())
        {
            auto const& pointer = work.begin()->second;
            work.erase(work.begin());
            do_gather_all_pointers_from_symbol(walker
                , pointer.type
                , pointer.type
                , pointer.variable_address
                , 0
                , pointers
                , {}
                , ignore_pointers
                , work);
        }
    }
}
