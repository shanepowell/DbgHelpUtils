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
#include "symbol_type_info.h"
#include "unloaded_module_list_stream.h"

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace dlg_help_utils::dbg_help;

namespace dlg_help_utils::symbol_type_utils
{
    namespace
    {
        std::unordered_map<sym_tag_enum, std::wstring_view> g_sym_tag_descriptions =
        {
            { sym_tag_enum::Null, L"(SymTagNull)"sv },
            { sym_tag_enum::Exe, L"exe"sv },
            { sym_tag_enum::Compiland, L"Compiland"sv },
            { sym_tag_enum::CompilandDetails, L"CompilandDetails"sv },
            { sym_tag_enum::CompilandEnv, L"CompilandEnv"sv },
            { sym_tag_enum::Function, L"Function"sv },
            { sym_tag_enum::Block, L"Block"sv },
            { sym_tag_enum::Data, L"Data"sv },
            { sym_tag_enum::Annotation, L"Annotation"sv },
            { sym_tag_enum::Label, L"Label"sv },
            { sym_tag_enum::PublicSymbol, L"PublicSymbol"sv },
            { sym_tag_enum::UDT, L"UserDefinedType"sv },
            { sym_tag_enum::Enum, L"ENUM"sv },
            { sym_tag_enum::FunctionType, L"FunctionType"sv },
            { sym_tag_enum::PointerType, L"PointerType"sv },
            { sym_tag_enum::ArrayType, L"ArrayType"sv },
            { sym_tag_enum::BaseType, L"BaseType"sv },
            { sym_tag_enum::Typedef, L"Typedef"sv },
            { sym_tag_enum::BaseClass, L"BaseClass"sv },
            { sym_tag_enum::Friend, L"Friend"sv },
            { sym_tag_enum::FunctionArgType, L"FunctionArgType"sv },
            { sym_tag_enum::FuncDebugStart, L"FuncDebugStart"sv },
            { sym_tag_enum::FuncDebugEnd, L"FuncDebugEnd"sv },
            { sym_tag_enum::UsingNamespace, L"UsingNamespace"sv },
            { sym_tag_enum::VTableShape, L"VTableShape"sv },
            { sym_tag_enum::VTable, L"VTable"sv },
            { sym_tag_enum::Custom, L"Custom"sv },
            { sym_tag_enum::Thunk, L"Thunk"sv },
            { sym_tag_enum::CustomType, L"CustomType"sv },
            { sym_tag_enum::ManagedType, L"ManagedType"sv },
            { sym_tag_enum::Dimension, L"Dimension"sv },
            { sym_tag_enum::CallSite, L"CallSite"sv },
            { sym_tag_enum::InlineSite, L"InlineSite"sv },
            { sym_tag_enum::BaseInterface, L"BaseInterface"sv },
            { sym_tag_enum::VectorType, L"VectorType"sv },
            { sym_tag_enum::MatrixType, L"MatrixType"sv },
            { sym_tag_enum::HLSLType, L"HLSLType"sv },
            { sym_tag_enum::Caller, L"Caller"sv },
            { sym_tag_enum::Callee, L"Callee"sv },
            { sym_tag_enum::Export, L"Export"sv },
            { sym_tag_enum::HeapAllocationSite, L"HeapAllocationSite"sv },
            { sym_tag_enum::CoffGroup, L"CoffGroup"sv },
            { sym_tag_enum::Inlinee, L"Inlinee"sv },
        };

        std::unordered_map<basic_type, std::wstring_view> g_basic_type_descriptions =
        {
            { basic_type::NoType, L"<NoType>"sv },
            { basic_type::Void, L"void"sv },
            { basic_type::Char, L"char"sv },
            { basic_type::WChar, L"wchar_t"sv },
            { basic_type::Int, L"int"sv },
            { basic_type::UInt, L"unsigned int"sv },
            { basic_type::Float, L"float"sv },
            { basic_type::BCD, L"<BCD>"sv },
            { basic_type::Bool, L"bool"sv },
            { basic_type::Long, L"long"sv },
            { basic_type::ULong, L"unsigned long"sv },
            { basic_type::Currency, L"<currency>"sv },
            { basic_type::Date, L"<date>"sv },
            { basic_type::Variant, L"VARIANT"sv },
            { basic_type::Complex, L"<complex>"sv },
            { basic_type::Bit, L"<bit>"sv },
            { basic_type::BSTR, L"BSTR"sv },
            { basic_type::HResult, L"HRESULT"sv },
            { basic_type::Char16, L"char16_t"sv },
            { basic_type::Char32, L"char32_t"sv },
            { basic_type::Char8, L"char8_t"sv },
        };

        std::unordered_map<udt_kind_type, std::wstring_view> g_udt_kind_type_descriptions =
        {
            { udt_kind_type::struct_type, L"struct"sv },
            { udt_kind_type::class_type, L"class"sv },
            { udt_kind_type::union_type, L"union"sv },
            { udt_kind_type::interface_type, L"interface"sv },
        };

        std::unordered_map<calling_convention, std::wstring_view> g_calling_convention_descriptions =
        {
            { calling_convention::near_c, L"near c"sv },
            { calling_convention::far_c, L"far c"sv },
            { calling_convention::near_pascal, L"near pascal"sv },
            { calling_convention::far_pascal, L"far pascal"sv },
            { calling_convention::near_fast, L"near fast"sv },
            { calling_convention::far_fast, L"far fast"sv },
            { calling_convention::skipped, L"skipped"sv },
            { calling_convention::near_std, L"near std"sv },
            { calling_convention::far_std, L"far std"sv },
            { calling_convention::near_sys, L"near sys"sv },
            { calling_convention::far_sys, L"far sys"sv },
            { calling_convention::thiscall, L"thiscall"sv },
            { calling_convention::mipscall, L"mipscall"sv },
            { calling_convention::generic, L"generic"sv },
            { calling_convention::alphacall, L"alphacall"sv },
            { calling_convention::ppccall, L"ppccall"sv },
            { calling_convention::shcall, L"shcall"sv },
            { calling_convention::armcall, L"armcall"sv },
            { calling_convention::am33call, L"am33call"sv },
            { calling_convention::tricall, L"tricall"sv },
            { calling_convention::sh5_call, L"sh5_call"sv },
            { calling_convention::m32rcall, L"m32rcall"sv },
            { calling_convention::clrcall, L"clrcall"sv },
            { calling_convention::inlinecall, L"inline"sv },
            { calling_convention::near_vector, L"near vector"sv },
            { calling_convention::swift, L"swift"sv },
            { calling_convention::reserved, L"reserved"sv },
        };

        std::unordered_map<data_kind, std::wstring_view> g_data_kind_descriptions =
        {
            { data_kind::data_is_local, L"local"sv },
            { data_kind::data_is_unknown, L"unknown"sv },
            { data_kind::data_is_static_local, L"static local"sv },
            { data_kind::data_is_param, L"param"sv },
            { data_kind::data_is_object_ptr, L"object ptr"sv },
            { data_kind::data_is_file_static, L"file static"sv },
            { data_kind::data_is_global, L"global"sv },
            { data_kind::data_is_member, L"member"sv },
            { data_kind::data_is_static_member, L"static member"sv },
            { data_kind::data_is_constant, L"constant"sv },
        };

        auto constexpr all_bits = std::numeric_limits<uint64_t>::max();
    }

    std::wstring_view sym_tag_to_string(sym_tag_enum const type)
    {
        if (auto const it = g_sym_tag_descriptions.find(type); it != g_sym_tag_descriptions.end())
        {
            return it->second;
        }

        static std::wstring bad_result;
        bad_result = std::format(L"unknown sym tag: {}", static_cast<int>(type));
        return bad_result;
    }

    std::wstring_view basic_type_to_string(basic_type const type)
    {
        if (auto const it = g_basic_type_descriptions.find(type); it != g_basic_type_descriptions.end())
        {
            return it->second;
        }

        static std::wstring bad_result;
        bad_result = std::format(L"unknown basic type: {}", static_cast<int>(type));
        return bad_result;
    }

    std::wstring_view udt_kind_to_string(udt_kind_type const type)
    {
        if (auto const it = g_udt_kind_type_descriptions.find(type); it != g_udt_kind_type_descriptions.end())
        {
            return it->second;
        }

        static std::wstring bad_result;
        bad_result = std::format(L"unknown UDT type: {}", static_cast<int>(type));
        return bad_result;
    }

    std::wstring_view calling_convention_to_string(calling_convention type)
    {
        if (auto const it = g_calling_convention_descriptions.find(type); it != g_calling_convention_descriptions.end())
        {
            return it->second;
        }

        static std::wstring bad_result;
        bad_result = std::format(L"unknown calling convention: {}", static_cast<int>(type));
        return bad_result;
    }

    std::wstring_view data_kind_convention_to_string(data_kind type)
    {
        if (auto const it = g_data_kind_descriptions.find(type); it != g_data_kind_descriptions.end())
        {
            return it->second;
        }

        static std::wstring bad_result;
        bad_result = std::format(L"unknown DataKind: ", static_cast<int>(type));
        return bad_result;
    }

    std::wstring_view get_symbol_name(symbol_type_info const& value)
    {
        auto const name_data = value.name();
        return name_data.value_or(L"(none)"sv);
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
                    name = udt_kind_to_string(kind_data.value());
                }

                if(!name.empty())
                {
                    name += L' ';
                }

                name += get_symbol_name(value);
                break;
            }

            case sym_tag_enum::Enum:
                name = L"enum "s;
                name += get_symbol_name(value);
                break;

            case sym_tag_enum::PointerType:
                if (auto const type_data = value.type(); type_data.has_value())
                {
                    name = get_symbol_type_friendly_name(type_data.value());
                    if (auto const reference_data = value.is_reference(); reference_data.has_value() && reference_data.value())
                    {
                        name += L"&"s;
                    }
                    else
                    {
                        name += L"*"s;
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
                                name = L"int8_t"s;
                                do_type = false;
                                break;

                            case 2:
                                name = L"int16_t"s;
                                do_type = false;
                                break;

                            case 4:
                                name = L"int32_t"s;
                                do_type = false;
                                break;

                            case 8:
                                name = L"int64_t"s;
                                do_type = false;
                                break;

                            case 16:
                                name = L"int128_t"s;
                                do_type = false;
                                break;

                            case 32:
                                name = L"int256_t"s;
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
                                name = L"uint8_t"s;
                                do_type = false;
                                break;

                            case 2:
                                name = L"uint16_t"s;
                                do_type = false;
                                break;

                            case 4:
                                name = L"uint32_t"s;
                                do_type = false;
                                break;

                            case 8:
                                name = L"uint64_t"s;
                                do_type = false;
                                break;

                            case 16:
                                name = L"uint128_t"s;
                                do_type = false;
                                break;

                            case 32:
                                name = L"uint256_t"s;
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
                                name = L"float"s;
                                do_type = false;
                                break;

                            case 8:
                                name = L"double"s;
                                do_type = false;
                                break;

                            case 16:
                                name = L"long double"s;
                                do_type = false;
                                break;

                            default:
                                break;
                            }
                        }
                    }

                    if (do_type)
                    {
                        name = basic_type_to_string(data_base_type.value());
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
                        name += calling_convention_to_string(calling_convention.value());
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
                name = sym_tag_to_string(tag_data.value());
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

    void dump_variable_type_at(std::wostream& os, mini_dump const& mini_dump, symbol_engine& symbol_engine, std::wstring const& symbol_type_name, uint64_t const variable_address, size_t const indent, size_t const visited_depth)
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
            os << std::format(L"failed to find [{}]\n", symbol_type_name);
            return;
        }

        auto const length = symbol_info.value().length();
        if(!length.has_value())
        {
            os << std::format(L"symbol [{}] is zero length\n", symbol_type_name);
            return;
        }

        auto stream = walker.get_process_memory_stream(variable_address, length.value());
        if(stream.eof())
        {
            os << std::format(L"failed to find [{0} - {1}] memory address range in dump file\n", stream_hex_dump::to_hex(variable_address), size_units::base_16::to_wstring(size_units::base_16::bytes{length.value()}));
            return;
        }

        dump_variable_symbol_at(os, walker, symbol_info.value(), symbol_info.value(), variable_address, stream, indent, visited_depth + 1);
    }

    void dump_variable_type_at(std::wostream& os, mini_dump const& mini_dump, symbol_engine& symbol_engine, symbol_type_info const& symbol_info, uint64_t const variable_address, size_t const indent, size_t const visited_depth)
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
            os << std::format(L"symbol [{}] is zero length\n", get_symbol_type_friendly_name(symbol_info));
            return;
        }

        auto stream = walker.get_process_memory_stream(variable_address, length.value());
        if(stream.eof())
        {
            os << std::format(L"failed to find [{0} - {1}] memory address range in dump file\n", stream_hex_dump::to_hex(variable_address), size_units::base_16::to_wstring(size_units::base_16::bytes{length.value()}));
            return;
        }

        dump_variable_symbol_at(os, walker, symbol_info, symbol_info, variable_address, stream, indent, visited_depth);
    }

    void dump_unsupported_variable_symbol_at(std::wostream& os, symbol_type_info const& type, sym_tag_enum const tag, [[maybe_unused]] uint64_t variable_address, [[maybe_unused]] mini_dump_memory_stream& variable_stream)
    {
        os << std::format(L" : unsupported type [{0}] on type [{1}]", sym_tag_to_string(tag), sym_tag_to_string(type.sym_tag().value_or(sym_tag_enum::Null)));
    }

    void dump_enum_variable_symbol_at(std::wostream& os, [[maybe_unused]] symbol_type_info const& type, [[maybe_unused]] uint64_t variable_address, [[maybe_unused]] mini_dump_memory_stream& variable_stream)
    {
        os << L" : enum value tbd";
    }

    template<typename T>
    void dump_array_inline(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t const variable_address, mini_dump_memory_stream& variable_stream, uint64_t max_size, bool const dump_hex = false)
    {
        if(max_size == 0)
        {
            max_size = walker.find_memory_range(variable_address, sizeof(T), 1);
        }

        print_utils::print_stream_array_inline<T>(os, variable_stream, static_cast<size_t>(max_size), dump_hex);
    }

    template<typename T>
    void dump_number_variable(std::wostream& os, [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t const variable_address, mini_dump_memory_stream& variable_stream, uint64_t const bit_mask, bool const is_pointer, size_t const max_size, bool const dump_hex, size_t const indent)
    {
        if(is_pointer)
        {
            if(max_size <= 5)
            {
                os << L": [";
                dump_array_inline<T>(os, walker, variable_address, variable_stream, max_size, dump_hex);
                os << L"]";
            }
            else
            {
                os << L":\n";
                print_utils::print_stream_array_lines<T>(os, variable_stream, max_size, 5, 20, indent, dump_hex);
            }
        }
        else 
        {
            T value;
            if(variable_stream.read(&value, sizeof(T)) == sizeof(T))
            {
                value &= static_cast<T>(bit_mask);
                if constexpr (std::is_floating_point_v<T>)
                {
                    os << std::format(L": {0}", locale_formatting::to_wstring(value));
                }
                else
                {
                    os << std::format(L": {0} ({1})", locale_formatting::to_wstring(value), stream_hex_dump::to_hex(value));
                }
            }
            else
            {
                os << L": <unknown>";
            }
        }
    }

    template<typename T>
    void dump_float_variable(std::wostream& os, [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t const variable_address, mini_dump_memory_stream& variable_stream, bool const is_pointer, size_t const max_size, size_t const indent)
    {
        if(is_pointer)
        {
            if(max_size <= 2)
            {
                os << L": [";
                dump_array_inline<T>(os, walker, variable_address, variable_stream, max_size);
                os << L"]";
            }
            else
            {
                os << L":\n";
                print_utils::print_stream_array_lines<T>(os, variable_stream, max_size, 2, 24, indent);
            }
        }
        else if(T value; variable_stream.read(&value, sizeof(T)) == sizeof(T))
        {
            os << std::format(L": {}", value);
        }
        else
        {
            os << L": <unknown>";
        }
    }

    template<typename T>
    void dump_string(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t variable_address, mini_dump_memory_stream& variable_stream, uint64_t max_size, uint64_t const limit_size = 256)
    {
        if(max_size == 0)
        {
            max_size = walker.find_memory_range_if(variable_address, sizeof(T), limit_size, [](void const* ptr) { return *static_cast<T const*>(ptr) == NULL; });
        }

        print_utils::print_stream_str<T>(os, variable_stream, static_cast<size_t>(max_size), false);
    }


    template<typename T>
    void dump_char_variable(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t const variable_address, mini_dump_memory_stream& variable_stream, bool const is_pointer, size_t const max_size)
    {
        if(is_pointer)
        {
            os << L": \"";
            dump_string<T>(os, walker, variable_address, variable_stream, max_size);
            os << L"\"";
        }
        else
        {
            T ch;
            if(variable_stream.read(&ch, sizeof ch) == sizeof ch)
            {
                os << std::format(L": {0} ({1})", print_utils::to_printable_char(ch), stream_hex_dump::to_hex(ch));
            }
        }
    }

    void dump_base_type_variable_symbol_at(std::wostream& os, [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, uint64_t const variable_address, mini_dump_memory_stream& variable_stream, uint64_t const bit_mask, bool const is_pointer, size_t const max_size, size_t const indent)
    {
        if(auto const base_type_data = type.base_type(); base_type_data.has_value())
        {
            switch(base_type_data.value())
            {
            case basic_type::Char:
                dump_char_variable<char>(os, walker, variable_address, variable_stream, is_pointer, max_size);
                break;

            case basic_type::WChar:
                dump_char_variable<wchar_t>(os, walker, variable_address, variable_stream, is_pointer, max_size);
                break;

            case basic_type::Int:
            case basic_type::Long:
                if(auto const length = type.length(); length.has_value())
                {
                    switch(length.value())
                    {
                    case 1:
                        dump_number_variable<int8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                        break;

                    case 2:
                        dump_number_variable<int16_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                        break;

                    case 4:
                        dump_number_variable<int32_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                        break;

                    case 8:
                        dump_number_variable<int64_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                        break;

                    default:
                        break;
                    }
                }
                break;

            case basic_type::UInt:
            case basic_type::ULong:
                if(auto const length = type.length(); length.has_value())
                {
                    switch(length.value())
                    {
                    case 1:
                        dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                        break;

                    case 2:
                        dump_number_variable<uint16_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                        break;

                    case 4:
                        dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                        break;

                    case 8:
                        dump_number_variable<uint64_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                        break;

                    default:
                        break;
                    }
                }
                break;

            case basic_type::Float:
                if(auto const length = type.length(); length.has_value())
                {
                    switch(length.value())
                    {
                    case 4:
                        dump_float_variable<float>(os, walker, variable_address, variable_stream, is_pointer, max_size, indent);
                        break;

                    case 8:
                        dump_float_variable<double>(os, walker, variable_address, variable_stream, is_pointer, max_size, indent);
                        break;

                    default:
                        break;
                    }
                }
                break;

            case basic_type::BCD:
                os << L": BCD value unsupported";
                break;

            case basic_type::Bool:
                dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                break;

            case basic_type::Currency:
                os << L": Currency value unsupported";
                break;

            case basic_type::Date:
                os << L": Date value unsupported";
                break;

            case basic_type::Variant:
                os << L": Variant value unsupported";
                break;

            case basic_type::Complex:
                os << L": Complex value unsupported";
                break;

            case basic_type::Bit:
                dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                break;

            case basic_type::BSTR:
                // ReSharper disable once StringLiteralTypo
                os << L": BSTR value unsupported";
                break;

            case basic_type::HResult:
                dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, false, indent);
                break;

            case basic_type::Char16:
                dump_char_variable<char16_t>(os, walker, variable_address, variable_stream, is_pointer, max_size);
                break;

            case basic_type::Char32:
                dump_char_variable<char32_t>(os, walker, variable_address, variable_stream, is_pointer, max_size);
                break;

            case basic_type::Char8:
                dump_char_variable<char8_t>(os, walker, variable_address, variable_stream, is_pointer, max_size);
                break;

            case basic_type::NoType:
            case basic_type::Void:
            default:  // NOLINT(clang-diagnostic-covered-switch-default)
                break;
            }
        }
    }

    void dump_udt_array(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, uint64_t variable_address, mini_dump_memory_stream& variable_stream, size_t const max_size, size_t const indent, size_t const visited_depth)
    {
        if(auto const length_data = type.length(); length_data.value_or(0) > 0)
        {
            auto const length = static_cast<size_t>(length_data.value());
            std::wstring const indent_str(indent, L' ');
            for(size_t index = 0; index < max_size; ++index)
            {
                os << std::format(L"{0}[{1}]\n", indent_str, locale_formatting::to_wstring(index));
                mini_dump_memory_stream copy_stream = variable_stream;
                dump_variable_symbol_at(os, walker, type, type, variable_address, copy_stream, indent, visited_depth + 1);
                variable_address += length_data.value();

                variable_stream.skip(length);
            }
        }
    }

    template<typename T>
    void dump_pointer_memory_value(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, T const& pointer_value, size_t const indent, size_t const visited_depth)
    {
        os << std::format(L": {}", stream_hex_dump::to_hex_full(pointer_value));
        if(auto const pointer_type = type.type(); pointer_type.has_value())
        {
            if(auto const length = pointer_type.value().length(); length.has_value() && length.value() > 0)
            {
                // lookup memory for pointer_value to see if it's in the memory list...
                if(auto variable_stream = walker.get_process_memory_stream(pointer_value, length.value()); !variable_stream.eof())
                {
                    if(auto const data_type_tag = pointer_type.value().sym_tag(); data_type_tag.has_value())
                    {
                        switch(data_type_tag.value())  // NOLINT(clang-diagnostic-switch-enum)
                        {
                            case sym_tag_enum::UDT:
                                if(visited_depth < 1)
                                {
                                    os << L'\n';
                                    dump_variable_symbol_at(os, walker, pointer_type.value(), pointer_type.value(), pointer_value, variable_stream, indent + 2, visited_depth + 1);
                                }
                                break;

                            case sym_tag_enum::BaseType:
                                dump_base_type_variable_symbol_at(os, walker, pointer_type.value(), pointer_value, variable_stream, all_bits, true, 0, indent);
                                break;

                            case sym_tag_enum::PointerType:
                                switch(length.value())
                                {
                                case 4:
                                    dump_number_variable<uint32_t>(os, walker, pointer_value, variable_stream, 0, true, 0, true, indent);
                                    /*
                                    os << L": [";
                                    dump_array_inline(os, walker, pointer_value, static_cast<uint32_t const*>(variable_memory), 0, true);
                                    os << L"]";
                                    */
                                    break;

                                case 8:
                                    dump_number_variable<uint64_t>(os, walker, pointer_value, variable_stream, 0, true, 0, true, indent);
                                    /*
                                    os << L": [";
                                    dump_array_inline(os, walker, pointer_value, static_cast<uint64_t const*>(variable_memory), 0, true);
                                    os << L"]";
                                    */
                                    break;

                                default:
                                    break;
                                }
                                break;

                            default:
                                break;
                        }
                    }
                }
            }
        }
    }

    void dump_pointer_variable_symbol_at(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, [[maybe_unused]] uint64_t variable_address, mini_dump_memory_stream& variable_stream, size_t const indent, size_t const visited_depth)
    {
        if(auto const length = type.length(); length.has_value())
        {
            switch(length.value())
            {
            case 4:
                if(uint32_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                {
                    dump_pointer_memory_value(os, walker, type, value, indent, visited_depth);
                }
                break;

            case 8:
                if(uint64_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                {
                    dump_pointer_memory_value(os, walker, type, value, indent, visited_depth);
                }
                break;

            default:
                break;
            }
        }
    }


    bool dump_array_variable_symbol_at(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, uint64_t const variable_address, [[maybe_unused]] mini_dump_memory_stream& variable_stream, size_t const indent, size_t const visited_depth)
    {
        if(auto const data_type = type.type(); data_type.has_value())
        {
            if(auto const data_type_tag = data_type.value().sym_tag(); data_type.has_value())
            {
                switch(data_type_tag.value())  // NOLINT(clang-diagnostic-switch-enum)
                {
                    case sym_tag_enum::BaseType:
                        dump_base_type_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream, all_bits, true, static_cast<size_t>(type.array_count().value_or(0)), indent);
                        break;

                    case sym_tag_enum::PointerType:
                        if(auto const length = data_type.value().length(); length.has_value())
                        {
                            switch(length.value())
                            {
                            case 4:
                                dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, 0, true, static_cast<size_t>(type.array_count().value_or(0)), true, indent);
                                /*
                                os << L": [";
                                dump_array_inline(os, walker, variable_address, static_cast<uint32_t const*>(variable_memory), static_cast<size_t>(type.array_count().value_or(0)), true);
                                os << L"]";
                                */
                                break;

                            case 8:
                                dump_number_variable<uint64_t>(os, walker, variable_address, variable_stream, 0, true, static_cast<size_t>(type.array_count().value_or(0)), true, indent);
                                /*
                                os << L": [";
                                dump_array_inline(os, walker, variable_address, static_cast<uint64_t const*>(variable_memory), static_cast<size_t>(type.array_count().value_or(0)), true);
                                os << L"]";
                                */
                                break;

                            default:
                                break;
                            }
                        }
                        break;

                    case sym_tag_enum::UDT:
                        os << L'\n';
                        dump_udt_array(os, walker, data_type.value(), variable_address, variable_stream, static_cast<size_t>(type.array_count().value_or(0)), indent, visited_depth);
                        return true;

                    default:
                        break;
                }
            }
        }
        return false;
    }

    void dump_bitmask(std::wostream& os, symbol_type_info const& type, unsigned long long& bit_mask)
    {
        if(auto const bit_position_data = type.bit_position(); bit_position_data.has_value())
        {
            if(auto const length_data = type.length();
                length_data.value_or(1) > 1)
            {
                bit_mask = (~(bit_mask << length_data.value())) << bit_position_data.value();
                os << std::format(L" bits {0}-{1} ({2})", bit_position_data.value(), bit_position_data.value() + length_data.value() - 1, stream_hex_dump::to_hex(bit_mask));
            }
            else
            {
                bit_mask = 0x01ULL << bit_position_data.value();
                os << std::format(L" bit {0} ({1})", bit_position_data.value(), stream_hex_dump::to_hex(bit_mask));
            }
        }
    }

    void dump_data_at(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, uint64_t const variable_address, mini_dump_memory_stream& variable_stream, std::optional<symbol_type_info> const data_type, std::optional<sym_tag_enum> const data_type_tag, bool& print_header, unsigned long long bit_mask, size_t const indent, size_t const visited_depth, dump_variable_symbol_options const options)
    {
        dump_bitmask(os, type, bit_mask);

        // data member, type the data member type and print based on that type
        if(data_type.has_value())
        {
            switch(auto const tag = data_type_tag.value_or(sym_tag_enum::Null); tag)  // NOLINT(clang-diagnostic-switch-enum)
            {
            case sym_tag_enum::UDT:
                if(print_header)
                {
                    os << L'\n';
                    print_header = false;
                }
                dump_variable_symbol_at(os, walker, data_type.value(), type, variable_address, variable_stream, indent, visited_depth + 1, options);
                break;

            case sym_tag_enum::Enum:
                dump_enum_variable_symbol_at(os, data_type.value(), variable_address, variable_stream);
                break;

            case sym_tag_enum::PointerType:
                dump_pointer_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream, indent, visited_depth);
                break;

            case sym_tag_enum::ArrayType:
                if(dump_array_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream, indent + 1, visited_depth))
                {
                    print_header = false;
                }
                break;

            case sym_tag_enum::BaseType:
                dump_base_type_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream, bit_mask, false, 0, indent);
                break;

            default:
                dump_unsupported_variable_symbol_at(os, data_type.value(), tag, variable_address, variable_stream);
                break;
            }
        }
    }

    void dump_point_type_at(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, uint64_t const variable_address, mini_dump_memory_stream& variable_stream, unsigned long long bit_mask, size_t const indent, size_t const visited_depth)
    {
        dump_bitmask(os, type, bit_mask);

        mini_dump_memory_stream copy_stream{variable_stream};
        dump_pointer_variable_symbol_at(os, walker, type, variable_address, copy_stream, indent, visited_depth);
    }

    bool can_dump_tag(sym_tag_enum const tag)
    {
        switch(tag)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case sym_tag_enum::Data:
        case sym_tag_enum::Enum:
        case sym_tag_enum::ArrayType:
        case sym_tag_enum::BaseType:
        case sym_tag_enum::PointerType:
        case sym_tag_enum::UDT:
            return true;

        default:
            return false;
        }
    }

    void dump_variable_symbol_at(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, symbol_type_info const& display_type, uint64_t const variable_address, [[maybe_unused]] mini_dump_memory_stream& variable_stream, size_t const indent, size_t const visited_depth, dump_variable_symbol_options const options)
    {
        auto const data_type = type.type();
        auto const data_type_tag = data_type.has_value() ? data_type.value().sym_tag() : std::nullopt;
        auto print_header = options == dump_variable_symbol_options::ForceHeader
                                || (options == dump_variable_symbol_options::AutoHeader && (visited_depth == 0 || data_type_tag.value_or(sym_tag_enum::Null) != sym_tag_enum::UDT));

        if(print_header)
        {
            os << std::format(L"{0:>{1}}{2} {3}", visited_depth == 0 ? L' ' : L'+', indent, stream_hex_dump::to_hex_full(variable_address), get_symbol_type_friendly_name(display_type));
        }

        mini_dump_memory_stream copy_variable_stream{variable_stream};
        switch(auto const tag = type.sym_tag().value_or(sym_tag_enum::Null); tag)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case sym_tag_enum::Data:
            dump_data_at(os, walker, type, variable_address, copy_variable_stream, data_type, data_type_tag, print_header, all_bits, indent, visited_depth, options);
            break;

        case sym_tag_enum::Enum:
            if(data_type.has_value())
            {
                dump_enum_variable_symbol_at(os, data_type.value(), variable_address, copy_variable_stream);
            }
            break;

        case sym_tag_enum::ArrayType:
            dump_array_variable_symbol_at(os, walker, type, variable_address, copy_variable_stream, indent + 1, visited_depth);
            break;

        case sym_tag_enum::BaseType:
            dump_base_type_variable_symbol_at(os, walker, type, variable_address, copy_variable_stream, all_bits, false, 0, indent);
            break;

        case sym_tag_enum::PointerType:
            dump_point_type_at(os, walker, type, variable_address, copy_variable_stream, all_bits, indent, visited_depth);
            break;

        case sym_tag_enum::UDT:
            if(data_type.has_value())
            {
                if(print_header)
                {
                    os << L'\n';
                }
                dump_variable_symbol_at(os, walker, type, data_type.value(), variable_address, copy_variable_stream, indent, visited_depth + 1);
            }
            break;

        default:
            dump_unsupported_variable_symbol_at(os, data_type.value_or(type), tag, variable_address, copy_variable_stream);
            break;
        }

        for (auto const& child : type.children())
        {
            if(auto const offset_data = child.offset(); offset_data.has_value())
            {
                if(auto const tag = child.sym_tag().value_or(sym_tag_enum::Null); can_dump_tag(tag))
                {
                    mini_dump_memory_stream copy_stream{variable_stream};
                    copy_stream.skip(offset_data.value());
                    os << L'\n';
                    dump_variable_symbol_at(os, walker, child, child, variable_address + offset_data.value(), copy_stream, indent + 1, visited_depth);
                }
            }
        }
    }

    uint64_t memory_address_from_string(std::wstring const& value)
    {
        size_t index{0};
        return std::stoull(value, &index, 0);
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
}
