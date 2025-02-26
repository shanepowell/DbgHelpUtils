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
        auto constexpr all_bits = std::numeric_limits<uint64_t>::max();

        bool any_variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , mini_dump_memory_stream& variable_stream);

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

        void dump_unsupported_variable_symbol_at(std::wostream& os, symbol_type_info const& type, sym_tag_enum const tag, [[maybe_unused]] uint64_t variable_address, [[maybe_unused]] mini_dump_memory_stream& variable_stream)
        {
            os << std::format(L" : {} [{}] {} [{}]", resources::get_unsupported_type(), resources::sym_tag_to_string(tag), resources::get_on_type(), resources::sym_tag_to_string(type.sym_tag().value_or(sym_tag_enum::Null)));
        }

        std::wstring generate_name_prefix(std::wstring const& name_prefix, symbol_type_info const& type)
        {
            if(name_prefix.empty())
            {
                return get_symbol_type_friendly_name(type);
            }

            return std::format(L"{0}.{1}", name_prefix, get_symbol_type_friendly_name(type));
        }

        template<typename T>
        void dump_array_inline(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t max_size
            , dump_hex_t const dump_hex = dump_hex_t{false})
        {
            if(max_size == 0)
            {
                max_size = walker.find_memory_range(variable_address, sizeof(T), 1);
            }

            print_utils::print_stream_array_inline<T>(os, variable_stream, static_cast<size_t>(max_size), dump_hex);
        }

        using is_pointer_t = tagged_bool<struct is_pointer_type>;

        template<typename T>
        void dump_number_variable(std::wostream& os
            , [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t const bit_mask
            , is_pointer_t const is_pointer
            , size_t const max_size
            , dump_hex_t const dump_hex
            , size_t const indent)
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
                T value{};
                if(variable_stream.read(&value, sizeof(T)) == sizeof(T))
                {
                    if constexpr (std::is_floating_point_v<T>)
                    {
                        os << std::format(L": {0}", locale_formatting::to_wstring(value));
                    }
                    else
                    {
                        value &= static_cast<T>(bit_mask);
                        os << std::format(L": {0} ({1})", locale_formatting::to_wstring(value), stream_hex_dump::to_hex(value));
                    }
                }
                else
                {
                    os << std::format(L": {}", resources::get_variable_unknown());
                }
            }
        }

        template<typename T>
        void single_line_dump_number_variable(std::wostream& os
            , [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t const bit_mask
            , is_pointer_t const is_pointer
            , size_t max_size
            , dump_hex_t const dump_hex)
        {
            if(is_pointer)
            {
                if(max_size == 0)
                {
                    max_size = static_cast<size_t>(walker.find_memory_range(variable_address, sizeof(T), 1));
                }

                if(max_size == 1)
                {
                    print_utils::print_stream_array_inline<T>(os, variable_stream, static_cast<size_t>(max_size), dump_hex);
                }
            }
            else 
            {
                T value{};
                if(variable_stream.read(&value, sizeof(T)) == sizeof(T))
                {
                    if constexpr (std::is_floating_point_v<T>)
                    {
                        os << std::format(L": {0}", locale_formatting::to_wstring(value));
                    }
                    else
                    {
                        value &= static_cast<T>(bit_mask);
                        os << std::format(L": {0} ({1})", locale_formatting::to_wstring(value), stream_hex_dump::to_hex(value));
                    }
                }
                else
                {
                    os << std::format(L": {}", resources::get_variable_unknown());
                }
            }
        }

        bool any_number_variable(stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t const variable_address
            , is_pointer_t const is_pointer
            , size_t const element_size
            , size_t max_size)
        {
            if (is_pointer)
            {
                if (max_size == 0)
                {
                    max_size = static_cast<size_t>(walker.find_memory_range(variable_address, element_size, 1));
                }

                if (max_size > 1)
                {
                    return true;
                }
            }

            return false;
        }

        template<typename T>
        std::generator<dump_variable_symbol_data>  number_variable(stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t const variable_address
            , mini_dump_memory_stream variable_stream
            , [[maybe_unused]] uint64_t const bit_mask
            , is_pointer_t const is_pointer
            , size_t max_size
            , dump_hex_t const dump_hex)
        {
            if(is_pointer)
            {
                if(max_size == 0)
                {
                    max_size = static_cast<size_t>(walker.find_memory_range(variable_address, sizeof(T), 1));
                }

                if(max_size > 1)
                {
                    for(uint64_t i = 0; i < max_size; ++i)
                    {
                        T value{};
                        if(variable_stream.read(&value, sizeof(T)) != sizeof(T))
                        {
                            break;
                        }

                        std::wostringstream ss;
                        if constexpr (std::is_floating_point_v<T>)
                        {
                            ss << std::to_wstring(value);
                        }
                        else
                        {
                            if(dump_hex)
                            {
                                ss << stream_hex_dump::to_hex_full(value, write_header_t{true});
                            }
                            else
                            {
                                ss << std::to_wstring(value);
                            }
                        }

                        co_yield dump_variable_symbol_data{.line= std::move(ss).str(), .sub_lines= {}};
                    }
                }
            }
        }


        void single_line_dump_enum_variable_symbol_at(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , mini_dump_memory_stream variable_stream)
        {
            if(auto const length = type.length(); length.has_value())
            {
                switch(length.value())
                {
                case 1:
                    single_line_dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, all_bits, is_pointer_t{false}, 1, dump_hex_t{false});
                    break;

                case 2:
                    single_line_dump_number_variable<uint16_t>(os, walker, variable_address, variable_stream, all_bits, is_pointer_t{false}, 1, dump_hex_t{false});
                    break;

                case 4:
                    single_line_dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, all_bits, is_pointer_t{false}, 1, dump_hex_t{false});
                    break;

                case 8:
                    single_line_dump_number_variable<uint64_t>(os, walker, variable_address, variable_stream, all_bits, is_pointer_t{false}, 1, dump_hex_t{false});
                    break;

                default:
                    os << std::format(L": ({})\n", length.value());
                    break;
                }
            }
        }

        template<typename T>
        void dump_string(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t max_size
            , uint64_t const limit_size = 256)
        {
            if(max_size == 0)
            {
                max_size = walker.find_memory_range_if(variable_address, sizeof(T), limit_size, [](void const* ptr) { return *static_cast<T const*>(ptr) == NULL; });
            }

            print_utils::print_stream_str<T>(os, variable_stream, static_cast<size_t>(max_size), print_utils::stop_at_null_t{false});
        }


        template<typename T>
        void dump_char_variable(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , is_pointer_t const is_pointer
            , size_t const max_size)
        {
            if(is_pointer)
            {
                os << L": \"";
                dump_string<T>(os, walker, variable_address, variable_stream, max_size);
                os << L"\"";
            }
            else
            {
                T ch{};
                if(variable_stream.read(&ch, sizeof ch) == sizeof ch)
                {
                    os << std::format(L": {0} ({1})", print_utils::to_printable_char(ch), stream_hex_dump::to_hex(ch));
                }
            }
        }

        void dump_base_type_variable_symbol_at(std::wostream& os
            , [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t const bit_mask
            , is_pointer_t const is_pointer
            , size_t const max_size
            , size_t const indent)
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
                            dump_number_variable<int8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                            break;

                        case 2:
                            dump_number_variable<int16_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                            break;

                        case 4:
                            dump_number_variable<int32_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                            break;

                        case 8:
                            dump_number_variable<int64_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
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
                            dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                            break;

                        case 2:
                            dump_number_variable<uint16_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                            break;

                        case 4:
                            dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                            break;

                        case 8:
                            dump_number_variable<uint64_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
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
                            dump_number_variable<float>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                            break;

                        case 8:
                            dump_number_variable<double>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                            break;

                        default:
                            break;
                        }
                    }
                    break;

                case basic_type::BCD:
                    os << std::format(L": {}", resources::get_bcd_value_unsupported());
                    break;

                case basic_type::Bool:
                    dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                    break;

                case basic_type::Currency:
                    os << std::format(L": {}", resources::get_currency_value_unsupported());
                    break;

                case basic_type::Date:
                    os << std::format(L": {}", resources::get_date_value_unsupported());
                    break;

                case basic_type::Variant:
                    os << std::format(L": {}", resources::get_variant_value_unsupported());
                    break;

                case basic_type::Complex:
                    os << std::format(L": {}", resources::get_complex_value_unsupported());
                    break;

                case basic_type::Bit:
                    dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
                    break;

                case basic_type::BSTR:
                    os << std::format(L": {}", resources::get_bstr_value_unsupported());
                    break;

                case basic_type::HResult:
                    dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}, indent);
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

        void single_line_dump_base_type_variable_symbol_at(std::wostream& os
            , [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , uint64_t const bit_mask
            , is_pointer_t const is_pointer
            , size_t const max_size)
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
                            single_line_dump_number_variable<int8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                            break;

                        case 2:
                            single_line_dump_number_variable<int16_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                            break;

                        case 4:
                            single_line_dump_number_variable<int32_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                            break;

                        case 8:
                            single_line_dump_number_variable<int64_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
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
                            single_line_dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                            break;

                        case 2:
                            single_line_dump_number_variable<uint16_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                            break;

                        case 4:
                            single_line_dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                            break;

                        case 8:
                            single_line_dump_number_variable<uint64_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
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
                            single_line_dump_number_variable<float>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                            break;

                        case 8:
                            single_line_dump_number_variable<double>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                            break;

                        default:
                            break;
                        }
                    }
                    break;

                case basic_type::BCD:
                    os << std::format(L": {}", resources::get_bcd_value_unsupported());
                    break;

                case basic_type::Bool:
                    single_line_dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                    break;

                case basic_type::Currency:
                    os << std::format(L": {}", resources::get_currency_value_unsupported());
                    break;

                case basic_type::Date:
                    os << std::format(L": {}", resources::get_date_value_unsupported());
                    break;

                case basic_type::Variant:
                    os << std::format(L": {}", resources::get_variant_value_unsupported());
                    break;

                case basic_type::Complex:
                    os << std::format(L": {}", resources::get_complex_value_unsupported());
                    break;

                case basic_type::Bit:
                    single_line_dump_number_variable<uint8_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
                    break;

                case basic_type::BSTR:
                    os << std::format(L": {}", resources::get_bstr_value_unsupported());
                    break;

                case basic_type::HResult:
                    single_line_dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false});
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

        bool any_base_type_variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , is_pointer_t const is_pointer
            , size_t const max_size)
        {
            if(auto const base_type_data = type.base_type(); base_type_data.has_value())
            {
                switch(base_type_data.value())
                {
                case basic_type::Char:
                case basic_type::WChar:
                    break;

                case basic_type::Int:
                case basic_type::Long:
                    if(auto const length = type.length(); length.has_value())
                    {
                        switch(length.value())
                        {
                        case 1:
                        case 2:
                        case 4:
                        case 8:
                            return any_number_variable(walker, variable_address, is_pointer, static_cast<size_t>(length.value()), max_size);

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
                        case 2:
                        case 4:
                        case 8:
                            return any_number_variable(walker, variable_address, is_pointer, static_cast<size_t>(length.value()), max_size);

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
                        case 8:
                            return any_number_variable(walker, variable_address, is_pointer, static_cast<size_t>(length.value()), max_size);

                        default:
                            break;
                        }
                    }
                    break;

                case basic_type::BCD:
                case basic_type::Bool:
                case basic_type::Currency:
                case basic_type::Date:
                case basic_type::Variant:
                case basic_type::Complex:
                case basic_type::Bit:
                case basic_type::BSTR:
                case basic_type::HResult:
                case basic_type::Char16:
                case basic_type::Char32:
                case basic_type::Char8:
                case basic_type::NoType:
                case basic_type::Void:
                default:  // NOLINT(clang-diagnostic-covered-switch-default)
                    break;
                }
            }

            return false;
        }

        std::generator<dump_variable_symbol_data> base_type_variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , mini_dump_memory_stream const& variable_stream
            , uint64_t const bit_mask
            , is_pointer_t const is_pointer
            , size_t const max_size)
        {
            if(auto const base_type_data = type.base_type(); base_type_data.has_value())
            {
                switch(base_type_data.value())
                {
                case basic_type::Char:
                case basic_type::WChar:
                    break;

                case basic_type::Int:
                case basic_type::Long:
                    if(auto const length = type.length(); length.has_value())
                    {
                        switch(length.value())
                        {
                        case 1:
                            for (auto&& data : number_variable<int8_t>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        case 2:
                            for (auto&& data : number_variable<int16_t>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        case 4:
                            for (auto&& data : number_variable<int32_t>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        case 8:
                            for (auto&& data : number_variable<int64_t>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
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
                            for (auto&& data : number_variable<uint8_t>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        case 2:
                            for (auto&& data : number_variable<uint16_t>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        case 4:
                            for (auto&& data : number_variable<uint32_t>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        case 8:
                            for (auto&& data : number_variable<uint64_t>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
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
                            for (auto&& data : number_variable<float>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        case 8:
                            for (auto&& data : number_variable<double>(walker, variable_address, variable_stream, bit_mask, is_pointer, max_size, dump_hex_t{false}))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        default:
                            break;
                        }
                    }
                    break;

                case basic_type::BCD:
                case basic_type::Bool:
                case basic_type::Currency:
                case basic_type::Date:
                case basic_type::Variant:
                case basic_type::Complex:
                case basic_type::Bit:
                case basic_type::BSTR:
                case basic_type::HResult:
                case basic_type::Char16:
                case basic_type::Char32:
                case basic_type::Char8:
                case basic_type::NoType:
                case basic_type::Void:
                default:  // NOLINT(clang-diagnostic-covered-switch-default)
                    break;
                }
            }
        }

        void dump_udt_array(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , size_t const max_size
            , size_t const indent
            , size_t const visited_depth)
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

        bool any_udt_array(symbol_type_info const& type, size_t const max_size)
        {
            auto const length_data = type.length();
            return length_data.value_or(0) > 0 && max_size > 0;
        }

        std::generator<dump_variable_symbol_data> udt_array(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream variable_stream
            , size_t const max_size
            , size_t const visited_depth)
        {
            if(auto const length_data = type.length(); length_data.value_or(0) > 0)
            {
                auto const length = static_cast<size_t>(length_data.value());
                for(size_t index = 0; index < max_size; ++index)
                {
                    if(mini_dump_memory_stream copy_variable_stream = variable_stream;
                        any_variable_symbol_at(walker, type, copy_variable_stream))
                    {
                        co_yield variable_symbol_at(walker, std::format(L"[{}] ", locale_formatting::to_wstring(index)), type, type, variable_address, variable_stream, visited_depth + 1);
                    }
                    else
                    {
                        co_yield dump_variable_symbol_data{ .line= std::format(L"[{}]", locale_formatting::to_wstring(index)), .sub_lines= {}};
                    }
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
                                    dump_base_type_variable_symbol_at(os, walker, pointer_type.value(), pointer_value, variable_stream, all_bits, is_pointer_t{true}, 0, indent);
                                    break;

                                case sym_tag_enum::PointerType:
                                    switch(length.value())
                                    {
                                    case 4:
                                        dump_number_variable<uint32_t>(os, walker, pointer_value, variable_stream, 0, is_pointer_t{true}, 0, dump_hex_t{true}, indent);
                                        /*
                                        os << L": [";
                                        dump_array_inline(os, walker, pointer_value, static_cast<uint32_t const*>(variable_memory), 0, true);
                                        os << L"]";
                                        */
                                        break;

                                    case 8:
                                        dump_number_variable<uint64_t>(os, walker, pointer_value, variable_stream, 0, is_pointer_t{true}, 0, dump_hex_t{true}, indent);
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

        template<typename T>
        bool any_pointer_memory_value(stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, T const& pointer_value)
        {
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
                                    return any_variable_symbol_at(walker, pointer_type.value(), variable_stream);

                                case sym_tag_enum::BaseType:
                                    return any_base_type_variable_symbol_at(walker, pointer_type.value(), pointer_value, is_pointer_t{ true }, 0);

                                case sym_tag_enum::PointerType:
                                    switch(length.value())
                                    {
                                    case 4:
                                    case 8:
                                        return any_number_variable(walker, pointer_value, is_pointer_t{ true }, static_cast<size_t>(length.value()), 0);

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

            return false;
        }

        template<typename T>
        std::generator<dump_variable_symbol_data> pointer_memory_value(stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, T const& pointer_value, size_t const visited_depth)
        {
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
                                    co_yield variable_symbol_at(walker, {}, pointer_type.value(), pointer_type.value(), pointer_value, variable_stream, visited_depth + 1);
                                    break;

                                case sym_tag_enum::BaseType:
                                    for (auto data : base_type_variable_symbol_at(walker, pointer_type.value(), pointer_value, variable_stream, all_bits, is_pointer_t{ true }, 0))  // NOLINT(performance-for-range-copy)
                                    {
                                        co_yield std::move(data);
                                    }
                                    break;

                                case sym_tag_enum::PointerType:
                                    switch(length.value())
                                    {
                                    case 4:
                                        for (auto data : number_variable<uint32_t>(walker, pointer_value, variable_stream, 0, is_pointer_t{true}, 0, dump_hex_t{true}))  // NOLINT(performance-for-range-copy)
                                        {
                                            co_yield std::move(data);
                                        }
                                        break;

                                    case 8:
                                        for (auto data : number_variable<uint64_t>(walker, pointer_value, variable_stream, 0, is_pointer_t{true}, 0, dump_hex_t{true}))  // NOLINT(performance-for-range-copy)
                                        {
                                            co_yield std::move(data);
                                        }
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


        template<typename T>
        void single_line_dump_pointer_memory_value(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, T const& pointer_value)
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
                                    break;

                                case sym_tag_enum::BaseType:
                                    single_line_dump_base_type_variable_symbol_at(os, walker, pointer_type.value(), pointer_value, variable_stream, all_bits, is_pointer_t{true}, 0);
                                    break;

                                case sym_tag_enum::PointerType:
                                    switch(length.value())
                                    {
                                    case 4:
                                        single_line_dump_number_variable<uint32_t>(os, walker, pointer_value, variable_stream, 0, is_pointer_t{true}, 0, dump_hex_t{true});
                                        break;

                                    case 8:
                                        single_line_dump_number_variable<uint64_t>(os, walker, pointer_value, variable_stream, 0, is_pointer_t{true}, 0, dump_hex_t{true});
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

        void dump_pointer_variable_symbol_at(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , [[maybe_unused]] uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , size_t const indent
            , size_t const visited_depth)
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


        bool any_pointer_variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , mini_dump_memory_stream& variable_stream)
        {
            if(auto const length = type.length(); length.has_value())
            {
                switch(length.value())
                {
                case 4:
                    if(uint32_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        return any_pointer_memory_value(walker, type, value);
                    }
                    break;

                case 8:
                    if(uint64_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        return any_pointer_memory_value(walker, type, value);
                    }
                    break;

                default:
                    break;
                }
            }

            return false;
        }

        std::generator<dump_variable_symbol_data> pointer_variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , [[maybe_unused]] uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , size_t const visited_depth)
        {
            if(auto const length = type.length(); length.has_value())
            {
                switch(length.value())
                {
                case 4:
                    if(uint32_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        for(auto&& data : pointer_memory_value(walker, type, value, visited_depth))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield data;
                        }
                    }
                    break;

                case 8:
                    if(uint64_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        for(auto&& data : pointer_memory_value(walker, type, value, visited_depth))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield data;
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }

        void single_line_dump_pointer_variable_symbol_at(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , [[maybe_unused]] uint64_t variable_address
            , mini_dump_memory_stream& variable_stream)
        {
            if(auto const length = type.length(); length.has_value())
            {
                switch(length.value())
                {
                case 4:
                    if(uint32_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        single_line_dump_pointer_memory_value(os, walker, type, value);
                    }
                    break;

                case 8:
                    if(uint64_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        single_line_dump_pointer_memory_value(os, walker, type, value);
                    }
                    break;

                default:
                    break;
                }
            }
        }

        bool dump_array_variable_symbol_at(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , [[maybe_unused]] mini_dump_memory_stream& variable_stream
            , size_t const indent
            , size_t const visited_depth)
        {
            if(auto const data_type = type.type(); data_type.has_value())
            {
                if(auto const data_type_tag = data_type.value().sym_tag(); data_type.has_value())
                {
                    switch(data_type_tag.value())  // NOLINT(clang-diagnostic-switch-enum)
                    {
                        case sym_tag_enum::BaseType:
                            dump_base_type_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream, all_bits, is_pointer_t{true}, static_cast<size_t>(type.array_count().value_or(0)), indent);
                            break;

                        case sym_tag_enum::PointerType:
                            if(auto const length = data_type.value().length(); length.has_value())
                            {
                                switch(length.value())
                                {
                                case 4:
                                    dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, 0, is_pointer_t{true}, static_cast<size_t>(type.array_count().value_or(0)), dump_hex_t{true}, indent);
                                    break;

                                case 8:
                                    dump_number_variable<uint64_t>(os, walker, variable_address, variable_stream, 0, is_pointer_t{true}, static_cast<size_t>(type.array_count().value_or(0)), dump_hex_t{true}, indent);
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

        void single_line_dump_array_variable_symbol_at(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , [[maybe_unused]] mini_dump_memory_stream& variable_stream)
        {
            if(auto const data_type = type.type(); data_type.has_value())
            {
                if(auto const data_type_tag = data_type.value().sym_tag(); data_type.has_value())
                {
                    switch(data_type_tag.value())  // NOLINT(clang-diagnostic-switch-enum)
                    {
                        case sym_tag_enum::BaseType:
                            single_line_dump_base_type_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream, all_bits, is_pointer_t{true}, static_cast<size_t>(type.array_count().value_or(0)));
                            break;

                        case sym_tag_enum::PointerType:
                            if(auto const length = data_type.value().length(); length.has_value())
                            {
                                switch(length.value())
                                {
                                case 4:
                                    single_line_dump_number_variable<uint32_t>(os, walker, variable_address, variable_stream, 0, is_pointer_t{true}, static_cast<size_t>(type.array_count().value_or(0)), dump_hex_t{true});
                                    break;

                                case 8:
                                    single_line_dump_number_variable<uint64_t>(os, walker, variable_address, variable_stream, 0, is_pointer_t{true}, static_cast<size_t>(type.array_count().value_or(0)), dump_hex_t{true});
                                    break;

                                default:
                                    break;
                                }
                            }
                            break;

                        case sym_tag_enum::UDT:  // NOLINT(bugprone-branch-clone)
                            break;

                        default:
                            break;
                    }
                }
            }
        }

        bool any_array_variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address)
        {
            if(auto const data_type = type.type(); data_type.has_value())
            {
                if(auto const data_type_tag = data_type.value().sym_tag(); data_type.has_value())
                {
                    switch(data_type_tag.value())  // NOLINT(clang-diagnostic-switch-enum)
                    {
                        case sym_tag_enum::BaseType:
                            return any_base_type_variable_symbol_at(walker, data_type.value(), variable_address, is_pointer_t{ true }, static_cast<size_t>(type.array_count().value_or(0)));

                        case sym_tag_enum::PointerType:
                            if(auto const length = data_type.value().length(); length.has_value())
                            {
                                switch(length.value())
                                {
                                case 4:
                                case 8:
                                    return any_number_variable(walker, variable_address, is_pointer_t{true}, static_cast<size_t>(length.value()), static_cast<size_t>(type.array_count().value_or(0)));

                                default:
                                    break;
                                }
                            }
                            break;

                    case sym_tag_enum::UDT:
                            return any_udt_array(data_type.value(), static_cast<size_t>(type.array_count().value_or(0)));

                        default:
                            break;
                    }
                }
            }

            return false;
        }

        std::generator<dump_variable_symbol_data> array_variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , [[maybe_unused]] mini_dump_memory_stream const& variable_stream
            , size_t const visited_depth)
        {
            if(auto const data_type = type.type(); data_type.has_value())
            {
                if(auto const data_type_tag = data_type.value().sym_tag(); data_type.has_value())
                {
                    switch(data_type_tag.value())  // NOLINT(clang-diagnostic-switch-enum)
                    {
                        case sym_tag_enum::BaseType:
                            for (auto&& data : base_type_variable_symbol_at(walker, data_type.value(), variable_address, variable_stream, all_bits, is_pointer_t{ true }, static_cast<size_t>(type.array_count().value_or(0))))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        case sym_tag_enum::PointerType:
                            if(auto const length = data_type.value().length(); length.has_value())
                            {
                                switch(length.value())
                                {
                                case 4:
                                    for (auto&& data : number_variable<uint32_t>(walker, variable_address, variable_stream, 0, is_pointer_t{true}, static_cast<size_t>(type.array_count().value_or(0)), dump_hex_t{true}))  // NOLINT(performance-for-range-copy)
                                    {
                                        co_yield data;
                                    }
                                    break;

                                case 8:
                                    for (auto&& data : number_variable<uint64_t>(walker, variable_address, variable_stream, 0, is_pointer_t{true}, static_cast<size_t>(type.array_count().value_or(0)), dump_hex_t{true}))  // NOLINT(performance-for-range-copy)
                                    {
                                        co_yield data;
                                    }
                                    break;

                                default:
                                    break;
                                }
                            }
                            break;

                    case sym_tag_enum::UDT:
                            for (auto&& data : udt_array(walker, data_type.value(), variable_address, variable_stream, static_cast<size_t>(type.array_count().value_or(0)), visited_depth))  // NOLINT(performance-for-range-copy)
                            {
                                co_yield data;
                            }
                            break;

                        default:
                            break;
                    }
                }
            }
        }

        void dump_bitmask(std::wostream& os, symbol_type_info const& type, unsigned long long& bit_mask)
        {
            if(auto const bit_position_data = type.bit_position(); bit_position_data.has_value())
            {
                if(auto const length_data = type.length();
                    length_data.value_or(1) > 1)
                {
                    bit_mask = (~(bit_mask << length_data.value())) << bit_position_data.value();
                    os << std::format(L" {} {}-{} ({})", resources::get_bits_title(), bit_position_data.value(), bit_position_data.value() + length_data.value() - 1, stream_hex_dump::to_hex(bit_mask));
                }
                else
                {
                    bit_mask = 0x01ULL << bit_position_data.value();
                    os << std::format(L" {} {} ({})", resources::get_bit_title(), bit_position_data.value(), stream_hex_dump::to_hex(bit_mask));
                }
            }
        }


        bool any_variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , mini_dump_memory_stream& variable_stream)
        {
            if(auto const length = type.length(); length.has_value())
            {
                switch(length.value())
                {
                case 4:
                    if(uint32_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        return any_pointer_memory_value(walker, type, value);
                    }
                    break;

                case 8:
                    if(uint64_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        return any_pointer_memory_value(walker, type, value);
                    }
                    break;

                default:
                    break;
                }
            }

            return false;
        }

        std::generator<dump_variable_symbol_data> variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , [[maybe_unused]] symbol_type_info const& display_type
            , [[maybe_unused]] uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , size_t const visited_depth)
        {
            if(auto const length = type.length(); length.has_value())
            {
                switch(length.value())
                {
                case 4:
                    if(uint32_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        for(auto&& data : pointer_memory_value(walker, type, value, visited_depth))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield data;
                        }
                    }
                    break;

                case 8:
                    if(uint64_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                    {
                        for(auto&& data : pointer_memory_value(walker, type, value, visited_depth))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield data;
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }

        void single_line_dump_data_at(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , std::optional<symbol_type_info> const& data_type
            , std::optional<sym_tag_enum> const data_type_tag
            , unsigned long long bit_mask)
        {
            dump_bitmask(os, type, bit_mask);

            // data member, type the data member type and print based on that type
            if(data_type.has_value())
            {
                switch(auto const tag = data_type_tag.value_or(sym_tag_enum::Null); tag)  // NOLINT(clang-diagnostic-switch-enum)
                {
                case sym_tag_enum::UDT:
                    break;

                case sym_tag_enum::Enum:
                    single_line_dump_enum_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream);
                    break;

                case sym_tag_enum::PointerType:
                    single_line_dump_pointer_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream);
                    break;

                case sym_tag_enum::ArrayType:
                    break;

                case sym_tag_enum::BaseType:
                    single_line_dump_base_type_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream, bit_mask, is_pointer_t{false}, 0);
                    break;

                default:
                    dump_unsupported_variable_symbol_at(os, data_type.value(), tag, variable_address, variable_stream);
                    break;
                }
            }
        }

        bool any_data_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , std::optional<symbol_type_info> const& data_type
            , std::optional<sym_tag_enum> const data_type_tag)
        {
            // data member, type the data member type and print based on that type
            if (data_type.has_value())
            {
                switch (auto const tag = data_type_tag.value_or(sym_tag_enum::Null); tag)  // NOLINT(clang-diagnostic-switch-enum)
                {
                case sym_tag_enum::UDT:
                    return any_variable_symbol_at(walker, data_type.value(), variable_stream);
                
                case sym_tag_enum::Enum:
                    break;

                case sym_tag_enum::PointerType:
                    return any_pointer_variable_symbol_at(walker, data_type.value(), variable_stream);

                case sym_tag_enum::ArrayType:
                    return any_array_variable_symbol_at(walker, data_type.value(), variable_address);

                case sym_tag_enum::BaseType:
                    return any_base_type_variable_symbol_at(walker, data_type.value(), variable_address, is_pointer_t{false}, 0);

                default:
                    break;
                }
            }
            return false;
        }

        std::generator<dump_variable_symbol_data> data_at(stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , std::optional<symbol_type_info> const& data_type
            , std::optional<sym_tag_enum> const data_type_tag
            , unsigned long long const bit_mask
            , size_t const visited_depth)
        {
            // data member, type the data member type and print based on that type
            if(data_type.has_value())
            {
                switch(auto const tag = data_type_tag.value_or(sym_tag_enum::Null); tag)  // NOLINT(clang-diagnostic-switch-enum)
                {
                case sym_tag_enum::UDT:
                    for (auto&& data : variable_symbol_at(walker, data_type.value(), type, variable_address, variable_stream, visited_depth + 1))  // NOLINT(performance-for-range-copy)
                    {
                        co_yield data;
                    }
                    break;

                case sym_tag_enum::Enum:
                    break;

                case sym_tag_enum::PointerType:
                    for(auto&& data : pointer_variable_symbol_at(walker, data_type.value(), variable_address, variable_stream, visited_depth))  // NOLINT(performance-for-range-copy)
                    {
                        co_yield data;
                    }
                    break;

                case sym_tag_enum::ArrayType:
                    for(auto&& data : array_variable_symbol_at(walker, data_type.value(), variable_address, variable_stream, visited_depth))  // NOLINT(performance-for-range-copy)
                    {
                        co_yield data;
                    }
                    break;

                case sym_tag_enum::BaseType:
                    for(auto&& data : base_type_variable_symbol_at(walker, data_type.value(), variable_address, variable_stream, bit_mask, is_pointer_t{false}, 0))  // NOLINT(performance-for-range-copy)
                    {
                        co_yield data;
                    }
                    break;

                default:
                    break;
                }
            }
        }
        void dump_data_at(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , mini_dump_memory_stream& variable_stream
            , std::optional<symbol_type_info> const& data_type
            , std::optional<sym_tag_enum> const data_type_tag
            , bool& print_header
            , unsigned long long bit_mask
            , size_t const indent
            , size_t const visited_depth
            , dump_variable_symbol_options const options)
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
                    single_line_dump_enum_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream);
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
                    dump_base_type_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream, bit_mask, is_pointer_t{false}, 0, indent);
                    break;

                default:
                    dump_unsupported_variable_symbol_at(os, data_type.value(), tag, variable_address, variable_stream);
                    break;
                }
            }
        }

        void dump_pointer_type_at(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , mini_dump_memory_stream const& variable_stream
            , unsigned long long bit_mask
            , size_t const indent
            , size_t const visited_depth)
        {
            dump_bitmask(os, type, bit_mask);

            mini_dump_memory_stream copy_stream{variable_stream};
            dump_pointer_variable_symbol_at(os, walker, type, variable_address, copy_stream, indent, visited_depth);
        }

        void single_line_dump_pointer_type_at(std::wostream& os
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_info const& type
            , uint64_t const variable_address
            , mini_dump_memory_stream const& variable_stream
            , unsigned long long bit_mask)
        {
            dump_bitmask(os, type, bit_mask);

            mini_dump_memory_stream copy_stream{variable_stream};
            single_line_dump_pointer_variable_symbol_at(os, walker, type, variable_address, copy_stream);
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
                    if(auto const tag = child.sym_tag().value_or(sym_tag_enum::Null); can_dump_tag(tag))
                    {
                        do_gather_all_pointers_from_symbol(walker, base_type, child, base_address + variable_address_offset, offset_data.value(), pointers, sub_name_prefix, ignore_pointers, work);
                    }
                }
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

    void dump_variable_type_at(std::wostream& os
        , mini_dump const& mini_dump
        , symbol_engine& symbol_engine
        , std::wstring const& symbol_type_name
        , uint64_t const variable_address
        , size_t const indent
        , size_t const visited_depth)
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

        dump_variable_symbol_at(os, walker, symbol_info.value(), symbol_info.value(), variable_address, stream, indent, visited_depth + 1);
    }

    void dump_variable_type_at(std::wostream& os
        , mini_dump const& mini_dump
        , symbol_engine& symbol_engine
        , symbol_type_info const& symbol_info
        , uint64_t const variable_address
        , size_t const indent
        , size_t const visited_depth)
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
            os << std::format(L"{} [{}] {}\n", resources::get_symbol_title(), get_symbol_type_friendly_name(symbol_info), resources::get_is_zero_length_string());
            return;
        }

        auto stream = walker.get_process_memory_stream(variable_address, length.value());
        if(stream.eof())
        {
            os << std::format(L"{} [{} - {}] {}\n", resources::get_failed_to_find_string(), stream_hex_dump::to_hex(variable_address), size_units::base_16::to_wstring(size_units::base_16::bytes{length.value()}), resources::get_memory_address_in_range_string());
            return;
        }

        dump_variable_symbol_at(os, walker, symbol_info, symbol_info, variable_address, stream, indent, visited_depth);
    }

    namespace
    {
        std::generator<dump_variable_symbol_data> children_variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker, symbol_type_info const& type, mini_dump_memory_stream const& variable_stream, uint64_t const variable_address, size_t const visited_depth)
        {
            for (auto const& child : type.children())
            {
                if (auto const offset_data = child.offset(); offset_data.has_value())
                {
                    if (auto const tag = child.sym_tag().value_or(sym_tag_enum::Null); can_dump_tag(tag))
                    {
                        mini_dump_memory_stream copy_stream{ variable_stream };
                        copy_stream.skip(offset_data.value());
                        for (auto&& field : variable_symbol_at(walker, child, child, variable_address + offset_data.value(), copy_stream, visited_depth))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield field;
                        }
                    }
                }
            }
        }
    }

    dump_variable_symbol_data variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
        , std::wstring_view const& prefix
        , symbol_type_info const& type
        , symbol_type_info const& display_type
        , uint64_t variable_address
        , mini_dump_memory_stream const& variable_stream
        , size_t visited_depth)
    {
        auto const data_type = type.type();
        auto const data_type_tag = data_type.has_value() ? data_type.value().sym_tag() : std::nullopt;

        std::wstringstream os;
        if(variable_address == 0)
        {
            os << std::format(L"{0} {1}", prefix, get_symbol_type_friendly_name(display_type));
        }
        else
        {
            os << std::format(L"{0}{1}{2} {3}", prefix, visited_depth == 0 ? L' ' : L'+', stream_hex_dump::to_hex_full(variable_address), get_symbol_type_friendly_name(display_type));
        }

        auto anyFields = false;
        for (auto const& child : type.children())
        {
            if(auto const offset_data = child.offset(); offset_data.has_value())
            {
                if(auto const tag = child.sym_tag().value_or(sym_tag_enum::Null); can_dump_tag(tag))
                {
                    anyFields = true;
                    break;
                }
            }
        }

        mini_dump_memory_stream copy_variable_stream{variable_stream};
        switch(auto const tag = type.sym_tag().value_or(sym_tag_enum::Null); tag)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case sym_tag_enum::Data:
            single_line_dump_data_at(os, walker, type, variable_address, copy_variable_stream, data_type, data_type_tag, all_bits);
            copy_variable_stream = variable_stream;
            if(any_data_at(walker, variable_address, copy_variable_stream, data_type, data_type_tag))
            {
                return dump_variable_symbol_data{.line= std::move(os).str(),
                    .sub_lines= [&walker,
                        &type,
                    variable_address,
                    data_type,
                    data_type_tag,
                    copy_variable_stream = variable_stream,
                    visited_depth]() mutable
                    {
                        return data_at(walker, type, variable_address, copy_variable_stream, data_type, data_type_tag, all_bits, visited_depth);
                    }
                };
            }
            break;

        case sym_tag_enum::Enum:
            if(data_type.has_value())
            {
                single_line_dump_enum_variable_symbol_at(os, walker, data_type.value(), variable_address, copy_variable_stream);
            }
            break;

        case sym_tag_enum::ArrayType:
            single_line_dump_array_variable_symbol_at(os, walker, type, variable_address, copy_variable_stream);
            if(any_array_variable_symbol_at(walker, type, variable_address))
            {
                return dump_variable_symbol_data{.line= std::move(os).str(),
                    .sub_lines= [&walker,
                        &type,
                    variable_address,
                    copy_variable_stream = variable_stream,
                    visited_depth]() mutable
                    {
                        return array_variable_symbol_at(walker, type, variable_address, copy_variable_stream, visited_depth);
                    }
                };
            }
            break;

        case sym_tag_enum::BaseType:
            single_line_dump_base_type_variable_symbol_at(os, walker, type, variable_address, copy_variable_stream, all_bits, is_pointer_t{false}, 0);
            break;

        case sym_tag_enum::PointerType:
            single_line_dump_pointer_type_at(os, walker, type, variable_address, copy_variable_stream, all_bits);
            if (any_pointer_memory_value(walker, type, variable_address))
            {
                return dump_variable_symbol_data{ .line= std::move(os).str(),
                    .sub_lines= [&walker,
                        &type,
                    variable_address,
                    copy_variable_stream = variable_stream,
                    visited_depth]() mutable
                    {
                        return pointer_variable_symbol_at(walker, type, variable_address, copy_variable_stream, visited_depth);
                    }
                };
            }
            break;

        case sym_tag_enum::UDT:
            if(data_type.has_value())
            {
                if(any_variable_symbol_at(walker, type, copy_variable_stream))
                {
                    return dump_variable_symbol_data{.line= std::move(os).str(),
                        .sub_lines= [&walker,
                            &type,
                        variable_address,
                        data_type,
                        copy_variable_stream = variable_stream,
                        visited_depth]() mutable
                        {
                            return variable_symbol_at(walker, type, data_type.value(), variable_address, copy_variable_stream, visited_depth + 1);
                        }
                    };
                }
            }
            break;

        default:
            dump_unsupported_variable_symbol_at(os, data_type.value_or(type), tag, variable_address, copy_variable_stream);
            break;
        }

        if(anyFields)
        {
            return dump_variable_symbol_data{.line= std::move(os).str(), .sub_lines = [&walker,
                &type,
                &variable_stream,
                variable_address,
                visited_depth]() mutable 
                {
                    return children_variable_symbol_at(walker, type, variable_stream, variable_address, visited_depth);
                }
            };
        }

        return dump_variable_symbol_data{.line= std::move(os).str(), .sub_lines = {}};
    }

    void dump_variable_symbol_at(std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_type_info const& type
        , symbol_type_info const& display_type
        , uint64_t const variable_address
        , [[maybe_unused]] mini_dump_memory_stream const& variable_stream
        , size_t const indent
        , size_t const visited_depth
        , dump_variable_symbol_options const options)
    {
        auto const data_type = type.type();
        auto const data_type_tag = data_type.has_value() ? data_type.value().sym_tag() : std::nullopt;
        auto print_header = options == dump_variable_symbol_options::ForceHeader
                                || (options == dump_variable_symbol_options::AutoHeader && (visited_depth == 0 || data_type_tag.value_or(sym_tag_enum::Null) != sym_tag_enum::UDT));

        if(print_header)
        {
            if(indent > 0)
            {
                os << std::format(L"{0:{1}}", ' ', indent);
            }
            os << std::format(L"{0}{1} {2}", visited_depth == 0 ? L' ' : L'+', stream_hex_dump::to_hex_full(variable_address), get_symbol_type_friendly_name(display_type));
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
                single_line_dump_enum_variable_symbol_at(os, walker, data_type.value(), variable_address, copy_variable_stream);
            }
            break;

        case sym_tag_enum::ArrayType:
            dump_array_variable_symbol_at(os, walker, type, variable_address, copy_variable_stream, indent + 1, visited_depth);
            break;

        case sym_tag_enum::BaseType:
            dump_base_type_variable_symbol_at(os, walker, type, variable_address, copy_variable_stream, all_bits, is_pointer_t{false}, 0, indent);
            break;

        case sym_tag_enum::PointerType:
            dump_pointer_type_at(os, walker, type, variable_address, copy_variable_stream, all_bits, indent, visited_depth);
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
