#include "symbol_data_dumper.h"

#include <format>
#include <sstream>

#include "dump_hex.h"
#include "locale_number_formatting.h"
#include "mini_dump_memory_stream.h"
#include "mini_dump_memory_walker.h"
#include "print_utils.h"
#include "stream_hex_dump.h"
#include "symbol_type_utils.h"

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace dlg_help_utils::dbg_help;

namespace dlg_help_utils::symbol_type_utils
{

    template<typename T>
    void symbol_data_dumper::dump_string(std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , uint64_t variable_address
        , mini_dump_memory_stream& variable_stream
        , uint64_t max_size
        , uint64_t const limit_size)
    {
        if(max_size == 0)
        {
            max_size = walker.find_memory_range_if(variable_address, sizeof(T), limit_size, [](void const* ptr) { return *static_cast<T const*>(ptr) == NULL; });
        }

        // ReSharper disable once CppRedundantCastExpression
        print_utils::print_stream_str<T>(os, variable_stream, static_cast<size_t>(max_size), print_utils::stop_at_null_t{false});
    }

    template<typename T>
    void symbol_data_dumper::dump_char_variable(std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , uint64_t const variable_address
        , mini_dump_memory_stream& variable_stream
        , is_pointer_t const is_pointer
        , size_t const max_size) const
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

    template<typename T>
    void symbol_data_dumper::dump_number_variable(std::wostream& os
        , [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker
        , mini_dump_memory_stream& variable_stream
        , uint64_t const bit_mask) const
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

    void symbol_data_dumper::dump_pointer_memory_value(
        std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , pointer_data_t const& pointer_data) const
    {
        os << std::format(L": {}", stream_hex_dump::to_hex(pointer_data.pointer_value, pointer_data.pointer_size * 2));
        if(auto const data_type_tag = pointer_data.pointer_type.sym_tag(); data_type_tag.has_value())
        {
            auto variable_stream = pointer_data.variable_stream;
            if(data_type_tag.value() == sym_tag_enum::BaseType)  // NOLINT(clang-diagnostic-switch-enum)
            {
                do_dump_base_type_variable_symbol_at(os, walker, pointer_data.pointer_type, pointer_data.pointer_value, variable_stream, g_all_bits, is_pointer_t{true}, 0);
            }
        }
    }

    dump_variable_symbol_data symbol_data_dumper::variable_symbol_at(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags options
        , std::wstring_view const& prefix
        , symbol_type_info const& type
        , symbol_type_info const& display_type
        , uint64_t variable_address
        , mini_dump_memory_stream const& variable_stream
        , std::wstring_view const& name
        , std::unordered_set<uint64_t>& visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info> const& parents) const
    {
        auto const data_type = type.type();
        auto const data_type_tag = data_type.has_value() ? data_type.value().sym_tag() : std::nullopt;

        auto print_header = (options & symbol_visit_flags::no_header) != symbol_visit_flags::no_header;

        std::wstringstream os;

        if (print_header)
        {
            if(variable_address == 0)
            {
                os << std::format(L"{0}{1}", prefix, get_symbol_type_friendly_name(display_type));
            }
            else
            {
                auto const pointer_size = get_pointer_size(options);
                os << std::format(L"{0}{1}{2} {3}", prefix, parents.empty() ? L"" : L"+", stream_hex_dump::to_hex(variable_address, pointer_size), get_symbol_type_friendly_name(display_type));
            }
        }
        else
        {
            options = static_cast<symbol_visit_flags::flags>(options & ~symbol_visit_flags::no_header);
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
            if (auto rv = dump_data_at(os, walker, options, type, tag, variable_address, copy_variable_stream, data_type, data_type_tag, g_all_bits, name, parents);
                rv.has_value())
            {
                return std::move(rv).value();
            }

            copy_variable_stream = variable_stream;
            if(any_data_at(os, walker, options, variable_address, copy_variable_stream, data_type, data_type_tag, visited_pointers))
            {
                return dump_variable_symbol_data{
                    .line= std::move(os).str(),
                    .sub_lines= [this,
                        &walker,
                        options,
                        type = type,
                        variable_address,
                        data_type = data_type,
                        data_type_tag = data_type_tag,
                        variable_stream = variable_stream,
                        name = std::wstring{name},
                        visited_pointers = visited_pointers,
                        max_symbol_dump_depth,
                        parents = parents]() mutable
                    {
                        return data_at(walker, options, type, variable_address, std::move(variable_stream), data_type, data_type_tag, std::move(name), std::move(visited_pointers), max_symbol_dump_depth, std::move(parents));
                    }
                };
            }
            break;

        case sym_tag_enum::Enum:
            if(data_type.has_value())
            {
                if (auto rv = dump_enum_variable_symbol_at(os, walker, data_type.value(), tag, variable_address, copy_variable_stream, name, parents);
                    rv.has_value())
                {
                    return std::move(rv).value();
                }
            }
            break;

        case sym_tag_enum::ArrayType:
            if (auto rv = dump_array_variable_symbol_at(os, walker, type, tag, variable_address, copy_variable_stream, name, parents);
                rv.has_value())
            {
                return std::move(rv).value();
            }
            if(any_array_variable_symbol_at(walker, type, variable_address))
            {
                return dump_variable_symbol_data
                {
                    .line= std::move(os).str(),
                    .sub_lines= [this,
                        &walker,
                        options,
                        type = type,
                        variable_address,
                        variable_stream = variable_stream,
                        name = std::wstring{name},
                        visited_pointers = visited_pointers,
                        max_symbol_dump_depth,
                        parents = parents]() mutable
                    {
                        return array_variable_symbol_at(walker, options, type, variable_address, std::move(variable_stream), std::move(name), std::move(visited_pointers), max_symbol_dump_depth, std::move(parents));
                    }
                };
            }
            break;

        case sym_tag_enum::BaseType:
            if (auto rv = dump_base_type_variable_symbol_at(os, walker, type, tag, variable_address, copy_variable_stream, g_all_bits, is_pointer_t{false}, 0, name, parents);
                rv.has_value())
            {
                return std::move(rv).value();
            }
            break;

        case sym_tag_enum::PointerType:
            if (auto rv = dump_pointer_type_at(os, walker, options, type, tag, variable_address, copy_variable_stream, g_all_bits, name, parents);
                rv.has_value())
            {
                return std::move(rv).value();
            }

            copy_variable_stream = variable_stream;
            if (any_pointer_memory_value(walker, options, type, variable_address, copy_variable_stream))
            {
                return dump_variable_symbol_data{
                    .line= std::move(os).str(),
                    .sub_lines= [this,
                        &walker,
                        options,
                        type = type,
                        variable_address,
                        variable_stream = variable_stream,
                        name = std::wstring{name},
                        visited_pointers = visited_pointers,
                        max_symbol_dump_depth,
                        parents = parents]() mutable
                    {
                        return pointer_variable_symbol_at(walker, options, type, variable_address, std::move(variable_stream), std::move(name), std::move(visited_pointers), max_symbol_dump_depth, std::move(parents));
                    }
                };
            }
            break;

        case sym_tag_enum::UDT:
            if(data_type.has_value())
            {
                if(any_variable_symbol_at(type))
                {
                    return dump_variable_symbol_data
                    {
                        .line= std::move(os).str(),
                        .sub_lines= [this,
                            &walker,
                            options,
                            type = type,
                            variable_address,
                            data_type = data_type.value(),
                            variable_stream = variable_stream,
                            name = std::wstring{name},
                            visited_pointers = visited_pointers,
                            max_symbol_dump_depth,
                            parents = parents]() mutable
                        {
                            return variable_symbol_udt_at(walker, options, type, data_type, variable_address, std::move(variable_stream), std::move(name), std::move(visited_pointers), max_symbol_dump_depth, std::move(parents));
                        }
                    };
                }
            }
            break;

        default:
            if (auto rv = dump_unsupported_variable_symbol_at(os, walker, data_type.value_or(type), tag, variable_address, copy_variable_stream, name, parents);
                rv.has_value())
            {
                return std::move(rv).value();
            }
            break;
        }

        if(anyFields && 
            (max_symbol_dump_depth == 0 || parents.size() < max_symbol_dump_depth))
        {
            std::vector child_parents{parents};
            child_parents.push_back(type);
            return dump_variable_symbol_data
            {
                .line= std::move(os).str(),
                .sub_lines = [
                    this,
                    &walker,
                    options,
                    type = type,
                    variable_stream = variable_stream,
                    variable_address,
                    visited_pointers = visited_pointers,
                    max_symbol_dump_depth,
                    child_parents = std::move(child_parents)]() mutable 
                {
                    return children_variable_symbol_at(walker, options, type, std::move(variable_stream), variable_address, std::move(visited_pointers), max_symbol_dump_depth, std::move(child_parents));
                }
            };
        }

        return dump_variable_symbol_data{.line= std::move(os).str(), .sub_lines = {}};
    }

    std::optional<dump_variable_symbol_data> symbol_data_dumper::dump_data_at(
        std::wstringstream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , sym_tag_enum tag
        , uint64_t const variable_address
        , mini_dump_memory_stream& variable_stream
        , std::optional<symbol_type_info> const& data_type
        , std::optional<sym_tag_enum> const& data_type_tag
        , unsigned long long bit_mask
        , std::wstring_view const& name
        , std::vector<symbol_type_info> const& parents) const
    {
        auto copy_variable_stream = variable_stream;
        std::wstringstream ss;
        dump_bitmask(ss, type, bit_mask);

        // data member, type the data member type and print based on that type
        if(data_type.has_value())
        {
            tag = data_type_tag.value_or(sym_tag_enum::Null);
            switch( tag)  // NOLINT(clang-diagnostic-switch-enum)
            {
            case sym_tag_enum::UDT:
                break;

            case sym_tag_enum::Enum:
                do_dump_enum_variable_symbol_at(ss, walker, data_type.value(), variable_stream);
                break;

            case sym_tag_enum::PointerType:
                do_dump_pointer_variable_symbol_at(ss, walker, options, data_type.value(), variable_stream);
                break;

            case sym_tag_enum::ArrayType:
                break;

            case sym_tag_enum::BaseType:
                do_dump_base_type_variable_symbol_at(ss, walker, data_type.value(), variable_address, variable_stream, bit_mask, is_pointer_t{false}, 0);
                break;

            default:
                do_dump_unsupported_variable_symbol_at(ss, data_type.value(), tag);
                break;
            }
        }

        return process_dump_value(os, walker, type, tag, variable_address, copy_variable_stream, name, parents, std::move(ss).str());
    }

    std::optional<dump_variable_symbol_data> symbol_data_dumper::dump_pointer_variable_symbol_at(
        std::wstringstream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , sym_tag_enum const tag
        , uint64_t const variable_address
        , mini_dump_memory_stream& variable_stream
        , std::wstring_view const& name
        , std::vector<symbol_type_info> const& parents) const
    {
        auto copy_variable_stream = variable_stream;
        std::wstringstream ss;

        do_dump_pointer_variable_symbol_at(ss, walker, options, type, variable_stream);

        return process_dump_value(os, walker, type, tag, variable_address, copy_variable_stream, name, parents, std::move(ss).str());
    }

    std::optional<dump_variable_symbol_data> symbol_data_dumper::dump_base_type_variable_symbol_at(
        std::wstringstream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_type_info const& type
        , sym_tag_enum const tag
        , uint64_t const variable_address
        , mini_dump_memory_stream& variable_stream
        , uint64_t const bit_mask
        , is_pointer_t const is_pointer
        , size_t const max_size
        , std::wstring_view const& name
        , std::vector<symbol_type_info> const& parents) const
    {
        auto copy_variable_stream = variable_stream;
        std::wstringstream ss;

        do_dump_base_type_variable_symbol_at(ss, walker, type, variable_address, variable_stream, bit_mask, is_pointer, max_size);

        return process_dump_value(os, walker, type, tag, variable_address, copy_variable_stream, name, parents, std::move(ss).str());
    }

    std::optional<dump_variable_symbol_data> symbol_data_dumper::dump_enum_variable_symbol_at(
        std::wstringstream& os, 
        stream_stack_dump::mini_dump_memory_walker const& walker, 
        symbol_type_info const& type,
        sym_tag_enum const tag,
        uint64_t const variable_address, 
        mini_dump_memory_stream const& variable_stream, 
        std::wstring_view const& name, 
        std::vector<symbol_type_info> const& parents) const
    {
        std::wstringstream ss;

        do_dump_enum_variable_symbol_at(ss, walker, type, variable_stream);

        return process_dump_value(os, walker, type, tag, variable_address, variable_stream, name, parents, std::move(ss).str());
    }

    std::optional<dump_variable_symbol_data> symbol_data_dumper::dump_array_variable_symbol_at(
        std::wstringstream& os, 
        stream_stack_dump::mini_dump_memory_walker const& walker, 
        symbol_type_info const& type,
        sym_tag_enum const tag,
        uint64_t const variable_address, 
        mini_dump_memory_stream& variable_stream, 
        std::wstring_view const& name, 
        std::vector<symbol_type_info> const& parents) const
    {
        auto copy_variable_stream = variable_stream;
        std::wstringstream ss;

        do_dump_array_variable_symbol_at(ss, walker, type, variable_address, variable_stream);

        return process_dump_value(os, walker, type, tag, variable_address, copy_variable_stream, name, parents, std::move(ss).str());
    }

    std::optional<dump_variable_symbol_data> symbol_data_dumper::dump_unsupported_variable_symbol_at(
        std::wstringstream& os, 
        stream_stack_dump::mini_dump_memory_walker const& walker, 
        symbol_type_info const& type, 
        sym_tag_enum const tag, 
        uint64_t const variable_address, 
        mini_dump_memory_stream const& variable_stream, 
        std::wstring_view const& name, 
        std::vector<symbol_type_info> const& parents) const
    {
        std::wstringstream ss;

        do_dump_unsupported_variable_symbol_at(ss, type, tag);

        return process_dump_value(os, walker, type, tag, variable_address, variable_stream, name, parents, std::move(ss).str());
    }

    std::optional<dump_variable_symbol_data> symbol_data_dumper::dump_pointer_type_at(
        std::wstringstream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , sym_tag_enum const tag
        , uint64_t const variable_address
        , mini_dump_memory_stream const& variable_stream
        , unsigned long long bit_mask
        , std::wstring_view const& name
        , std::vector<symbol_type_info> const& parents) const
    {
        std::wstringstream ss;

        dump_bitmask(ss, type, bit_mask);

        mini_dump_memory_stream copy_stream{variable_stream};
        do_dump_pointer_variable_symbol_at(ss, walker, options, type, copy_stream);

        return process_dump_value(os, walker, type, tag, variable_address, variable_stream, name, parents, std::move(ss).str());
    }

    void symbol_data_dumper::register_custom_type_formatter(std::unique_ptr<symbol_type_custom_formatter> formatter)
    {
        if (formatter)
        {
            custom_formatters_.emplace_back(std::move(formatter));
        }
    }

    void symbol_data_dumper::do_dump_pointer_variable_symbol_at(
        std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , mini_dump_memory_stream& variable_stream) const
    {
        if (auto pointer_data = get_pointer(walker, options, type, variable_stream);
            pointer_data.has_value())
        {
            dump_pointer_memory_value(os, walker, pointer_data.value());
        }
    }

    void symbol_data_dumper::do_dump_base_type_variable_symbol_at(
        std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_type_info const& type
        , uint64_t const variable_address
        , mini_dump_memory_stream& variable_stream
        , uint64_t const bit_mask
        , is_pointer_t const is_pointer
        , size_t const max_size) const
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
                        dump_number_variable<int8_t>(os, walker, variable_stream, bit_mask);
                        break;

                    case 2:
                        dump_number_variable<int16_t>(os, walker, variable_stream, bit_mask);
                        break;

                    case 4:
                        dump_number_variable<int32_t>(os, walker, variable_stream, bit_mask);
                        break;

                    case 8:
                        dump_number_variable<int64_t>(os, walker, variable_stream, bit_mask);
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
                        dump_number_variable<uint8_t>(os, walker, variable_stream, bit_mask);
                        break;

                    case 2:
                        dump_number_variable<uint16_t>(os, walker, variable_stream, bit_mask);
                        break;

                    case 4:
                        dump_number_variable<uint32_t>(os, walker, variable_stream, bit_mask);
                        break;

                    case 8:
                        dump_number_variable<uint64_t>(os, walker, variable_stream, bit_mask);
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
                        dump_number_variable<float>(os, walker, variable_stream, bit_mask);
                        break;

                    case 8:
                        dump_number_variable<double>(os, walker, variable_stream, bit_mask);
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
                dump_number_variable<uint8_t>(os, walker, variable_stream, bit_mask);
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
                dump_number_variable<uint8_t>(os, walker, variable_stream, bit_mask);
                break;

            case basic_type::BSTR:
                os << std::format(L": {}", resources::get_bstr_value_unsupported());
                break;

            case basic_type::HResult:
                dump_number_variable<uint32_t>(os, walker, variable_stream, bit_mask);
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

    void symbol_data_dumper::do_dump_enum_variable_symbol_at(
        std::wostream& os, 
        stream_stack_dump::mini_dump_memory_walker const& walker, 
        symbol_type_info const& type, 
        mini_dump_memory_stream variable_stream) const
    {
        if(auto const length = type.length(); length.has_value())
        {
            switch(length.value())
            {
            case 1:
                dump_number_variable<uint8_t>(os, walker, variable_stream, g_all_bits);
                break;

            case 2:
                dump_number_variable<uint16_t>(os, walker, variable_stream, g_all_bits);
                break;

            case 4:
                dump_number_variable<uint32_t>(os, walker, variable_stream, g_all_bits);
                break;

            case 8:
                dump_number_variable<uint64_t>(os, walker, variable_stream, g_all_bits);
                break;

            default:
                os << std::format(L": ({})\n", length.value());
                break;
            }
        }
    }

    void symbol_data_dumper::do_dump_array_variable_symbol_at(
        std::wostream& os, 
        stream_stack_dump::mini_dump_memory_walker const& walker, 
        symbol_type_info const& type, 
        uint64_t const variable_address, 
        mini_dump_memory_stream& variable_stream) const
    {
        if(auto const data_type = type.type(); data_type.has_value())
        {
            if(auto const data_type_tag = data_type.value().sym_tag(); data_type.has_value())
            {
                if(data_type_tag.value() == sym_tag_enum::BaseType)
                {
                    do_dump_base_type_variable_symbol_at(os, walker, data_type.value(), variable_address, variable_stream, g_all_bits, is_pointer_t{true}, static_cast<size_t>(type.array_count().value_or(0)));
                }
            }
        }
    }

    void symbol_data_dumper::do_dump_unsupported_variable_symbol_at(
        std::wostream& os,
        symbol_type_info const& type,
        sym_tag_enum const tag)
    {
        os << std::format(L" : {} [{}] {} [{}]", resources::get_unsupported_type(), resources::sym_tag_to_string(tag), resources::get_on_type(), resources::sym_tag_to_string(type.sym_tag().value_or(sym_tag_enum::Null)));
    }

    std::optional<dump_variable_symbol_data> symbol_data_dumper::process_dump_value(
        std::wstringstream& os, 
        stream_stack_dump::mini_dump_memory_walker const& walker, 
        symbol_type_info const& type,
        sym_tag_enum const tag,
        uint64_t const variable_address, 
        mini_dump_memory_stream const& variable_stream, 
        std::wstring_view const& name, 
        std::vector<symbol_type_info> const& parents, 
        std::wstring_view const& original_value) const
    {
        auto rv = symbol_type_custom_formatter_result::process;
        std::function<generator<dump_variable_symbol_data>()> sub_lines;

        // if we have a custom formatter, use that
        for (auto& formatter : custom_formatters_)
        {
            if (formatter->is_custom_type(type))
            {
                auto copy_variable_stream = variable_stream;
                rv = formatter->format(os, sub_lines, original_value, walker, type, tag, variable_address, copy_variable_stream, name, parents);

                if (rv != symbol_type_custom_formatter_result::process)
                {
                    break;
                }
            }
        }

        if (rv == symbol_type_custom_formatter_result::process)
        {
            os << original_value;
            return std::nullopt;
        }

        if (rv == symbol_type_custom_formatter_result::stop)
        {
            return dump_variable_symbol_data
            {
                .line= std::move(os).str(),
                .sub_lines= std::move(sub_lines)
            };
        }

        return std::nullopt;
    }

    generator<dump_variable_symbol_data> symbol_data_dumper::data_at(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const type  // NOLINT(performance-unnecessary-value-param)
        , uint64_t const variable_address
        , mini_dump_memory_stream variable_stream
        , std::optional<symbol_type_info> const data_type  // NOLINT(performance-unnecessary-value-param)
        , std::optional<sym_tag_enum> const data_type_tag
        , std::wstring name
        , std::unordered_set<uint64_t> visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info> parents) const
    {
        // data member, type the data member type and print based on that type
        if(data_type.has_value())
        {
            switch(auto const tag = data_type_tag.value_or(sym_tag_enum::Null); tag)  // NOLINT(clang-diagnostic-switch-enum)
            {
            case sym_tag_enum::UDT:
                for (auto&& data : variable_symbol_udt_at(walker, options, type, data_type.value(), variable_address, std::move(variable_stream), std::move(name), std::move(visited_pointers), max_symbol_dump_depth, std::move(parents)))  // NOLINT(performance-for-range-copy)
                {
                    co_yield std::move(data);
                }
                break;

            case sym_tag_enum::Enum:
                break;

            case sym_tag_enum::PointerType:
                for(auto&& data : pointer_variable_symbol_at(walker, options, data_type.value(), variable_address, std::move(variable_stream), std::move(name), std::move(visited_pointers), max_symbol_dump_depth, std::move(parents)))  // NOLINT(performance-for-range-copy)
                {
                    co_yield std::move(data);
                }
                break;

            case sym_tag_enum::ArrayType:
                for(auto&& data : array_variable_symbol_at(walker, options, data_type.value(), variable_address, std::move(variable_stream), std::move(name), std::move(visited_pointers), max_symbol_dump_depth, std::move(parents)))  // NOLINT(performance-for-range-copy)
                {
                    co_yield std::move(data);
                }
                break;

            case sym_tag_enum::BaseType:
                for(auto&& data : base_type_variable_symbol_at(walker, options, data_type.value(), tag, variable_address, variable_stream, is_pointer_t{false}, 0, name, visited_pointers, max_symbol_dump_depth, parents))  // NOLINT(performance-for-range-copy)
                {
                    co_yield std::move(data);
                }
                break;

            default:
                break;
            }
        }
    }

    generator<dump_variable_symbol_data> symbol_data_dumper::array_variable_symbol_at(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const type  // NOLINT(performance-unnecessary-value-param)
        , uint64_t const variable_address
        , [[maybe_unused]] mini_dump_memory_stream const variable_stream  // NOLINT(performance-unnecessary-value-param)
        , std::wstring const name  // NOLINT(performance-unnecessary-value-param)
        , std::unordered_set<uint64_t> visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info> parents) const
    {
        if(auto const data_type = type.type(); 
            data_type.has_value())
        {
            if(auto const data_type_tag = data_type.value().sym_tag(); 
                data_type.has_value() &&
                (max_symbol_dump_depth == 0 || parents.size() < max_symbol_dump_depth))
            {
                parents.push_back(type);
                switch(data_type_tag.value())  // NOLINT(clang-diagnostic-switch-enum)
                {
                    case sym_tag_enum::BaseType:
                        for (auto&& data : base_type_variable_symbol_at(walker, options, data_type.value(), data_type_tag.value(), variable_address, variable_stream, is_pointer_t{ true }, static_cast<size_t>(type.array_count().value_or(0)), name, visited_pointers, max_symbol_dump_depth, parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
                        }
                        break;

                    case sym_tag_enum::PointerType:
                        if(auto const length = data_type.value().length(); length.has_value())
                        {
                            switch(length.value())
                            {
                            case 4:
                                for (auto&& data : variable_pointer_array<uint32_t>(walker,
                                    options,
                                    type,
                                    variable_address, 
                                    variable_stream,
                                    is_pointer_t{true}, 
                                    data_type, 
                                    data_type_tag, 
                                    static_cast<size_t>(type.array_count().value_or(0)), 
                                    dump_hex_t{true},
                                    name,
                                    visited_pointers,
                                    max_symbol_dump_depth,
                                    parents))  // NOLINT(performance-for-range-copy)
                                {
                                    co_yield std::move(data);
                                }
                                break;

                            case 8:
                                for (auto&& data : variable_pointer_array<uint64_t>(walker,
                                    options,
                                    type,
                                    variable_address, 
                                    variable_stream, 
                                    is_pointer_t{true}, 
                                    data_type, 
                                    data_type_tag, 
                                    static_cast<size_t>(type.array_count().value_or(0)), 
                                    dump_hex_t{true}, 
                                    name,
                                    visited_pointers,
                                    max_symbol_dump_depth,
                                    parents))  // NOLINT(performance-for-range-copy)
                                {
                                    co_yield std::move(data);
                                }
                                break;

                            default:
                                break;
                            }
                        }
                        break;

                case sym_tag_enum::UDT:
                        for (auto&& data : dump_udt_array(walker, options, data_type.value(), variable_address, variable_stream, static_cast<size_t>(type.array_count().value_or(0)), name, visited_pointers, max_symbol_dump_depth, parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
                        }
                        break;

                    default:
                        break;
                }
            }
        }
    }

    generator<dump_variable_symbol_data> symbol_data_dumper::pointer_variable_symbol_at(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info type
        , [[maybe_unused]] uint64_t variable_address
        , mini_dump_memory_stream variable_stream
        , std::wstring name
        , std::unordered_set<uint64_t> visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info> parents) const
    {
        if (auto pointer_data = get_pointer(walker, options, type, variable_stream);
            pointer_data.has_value())
        {
            for(auto&& data : pointer_memory_value(walker, 
                options, 
                type, 
                pointer_data.value().pointer_value, 
                pointer_data.value().pointer_type, 
                pointer_data.value().variable_stream, 
                name, 
                visited_pointers,
                max_symbol_dump_depth,
                parents))  // NOLINT(performance-for-range-copy)
            {
                co_yield data;
            }
        }
    }

    generator<dump_variable_symbol_data> symbol_data_dumper::variable_symbol_udt_at(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const type  // NOLINT(performance-unnecessary-value-param)
        , [[maybe_unused]] symbol_type_info const display_type  // NOLINT(performance-unnecessary-value-param)
        , uint64_t const variable_address
        , mini_dump_memory_stream const variable_stream  // NOLINT(performance-unnecessary-value-param)
        , std::wstring const name  // NOLINT(performance-unnecessary-value-param)
        , std::unordered_set<uint64_t> visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info> parents) const
    {
        for(auto&& data : pointer_memory_value(walker, options, type, variable_address, display_type, variable_stream, name, visited_pointers, max_symbol_dump_depth, parents))  // NOLINT(performance-for-range-copy)
        {
            co_yield std::move(data);
        }
    }

    generator<dump_variable_symbol_data> symbol_data_dumper::children_variable_symbol_at(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const type
        , mini_dump_memory_stream const variable_stream
        , uint64_t const variable_address
        , std::unordered_set<uint64_t> visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info> const parents) const
    {
        for (auto const& child : type.children())
        {
            if (auto const offset_data = child.offset(); offset_data.has_value())
            {
                if (auto const tag = child.sym_tag().value_or(sym_tag_enum::Null); can_dump_tag(tag))
                {
                    mini_dump_memory_stream copy_stream{ variable_stream };
                    copy_stream.skip(offset_data.value());
                    co_yield variable_symbol_at(walker, options, {}, child, child, variable_address + offset_data.value(), copy_stream, child.best_name(), visited_pointers, max_symbol_dump_depth, parents);
                }
            }
        }
    }

    generator<dump_variable_symbol_data> symbol_data_dumper::base_type_variable_symbol_at(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , sym_tag_enum tag
        , uint64_t const variable_address
        , mini_dump_memory_stream const& variable_stream
        , is_pointer_t const is_pointer
        , size_t const max_size
        , std::wstring_view const& name
        , std::unordered_set<uint64_t>& visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info> const& parents) const
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
                        for (auto&& data : variable_pointer_array<int8_t>(
                            walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
                        }
                        break;

                    case 2:
                        for (auto&& data : variable_pointer_array<int16_t>(
                            walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
                        }
                        break;

                    case 4:
                        for (auto&& data : variable_pointer_array<int32_t>(
                            walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
                        }
                        break;

                    case 8:
                        for (auto&& data : variable_pointer_array<int64_t>(
                            walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
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
                        for (auto&& data : variable_pointer_array<uint8_t>(
                                                        walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
                        }
                        break;

                    case 2:
                        for (auto&& data : variable_pointer_array<uint16_t>(
                            walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
                        }
                        break;

                    case 4:
                        for (auto&& data : variable_pointer_array<uint32_t>(
                            walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
                        }
                        break;

                    case 8:
                        for (auto&& data : variable_pointer_array<uint64_t>(
                            walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
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
                        for (auto&& data : variable_pointer_array<float>(
                            walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
                        }
                        break;

                    case 8:
                        for (auto&& data : variable_pointer_array<double>(
                            walker, 
                            options, 
                            type, 
                            variable_address, 
                            variable_stream, 
                            is_pointer, 
                            type, 
                            tag, 
                            max_size, 
                            dump_hex_t{false}, 
                            name, 
                            visited_pointers,
                            max_symbol_dump_depth,
                            parents))  // NOLINT(performance-for-range-copy)
                        {
                            co_yield std::move(data);
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

    template<typename T>
    generator<dump_variable_symbol_data> symbol_data_dumper::variable_pointer_array(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , uint64_t const variable_address
        , mini_dump_memory_stream variable_stream
        , is_pointer_t const is_pointer
        , std::optional<symbol_type_info> const& data_type
        , std::optional<sym_tag_enum> const data_type_tag
        , size_t max_size
        , dump_hex_t const dump_hex
        , std::wstring_view const& name
        , std::unordered_set<uint64_t>& visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info> parents) const
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

                    auto indexName = std::format(L"{}[{}] ", name, i);
                    auto const pointer_size = get_pointer_size(options);
                    auto indexPrefix = std::format(L"+{} [{}] ", stream_hex_dump::to_hex(sizeof(T) * i, pointer_size), i);
                    std::wostringstream ss;

                    if constexpr (std::is_floating_point_v<T>)
                    {
                        ss << indexPrefix << std::to_wstring(value);
                    }
                    else
                    {
                        if(dump_hex)
                        {
                            ss << indexPrefix << stream_hex_dump::to_hex_full(value, write_header_t{true});
                        }
                        else
                        {
                            ss << indexPrefix << std::to_wstring(value);
                        }
                    }

                    co_yield dump_variable_symbol_data{
                        .line= std::move(ss).str(),
                        .sub_lines= 
                        {
                            [this,
                            &walker,
                            options,
                            type = type,
                            variable_address,
                            data_type = data_type,
                            data_type_tag = data_type_tag,
                            variable_stream = variable_stream,
                            visited_pointers = visited_pointers,
                            name = std::move(indexName),
                            max_symbol_dump_depth,
                            parents = parents]() mutable
                            {
                                return data_at(walker, options, type, variable_address, std::move(variable_stream), data_type, data_type_tag, std::move(name), std::move(visited_pointers), max_symbol_dump_depth, std::move(parents));
                            }
                        }
                    };
                }
            }
        }
    }

    generator<dump_variable_symbol_data> symbol_data_dumper::dump_udt_array(
        stream_stack_dump::mini_dump_memory_walker const&walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , uint64_t variable_address
        , mini_dump_memory_stream variable_stream
        , size_t const max_size
        , std::wstring_view const& name
        , std::unordered_set<uint64_t>& visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info> parents) const
    {
        if(auto const length_data = type.length(); 
            length_data.value_or(0) > 0 && 
            (max_symbol_dump_depth == 0 || parents.size() < max_symbol_dump_depth))
        {
            parents.push_back(type);
            auto const length = static_cast<size_t>(length_data.value());
            for(size_t index = 0; index < max_size; ++index)
            {
                if(any_variable_symbol_at(type))
                {
                    auto indexName = std::format(L"{}[{}] ", name, index);
                    auto const pointer_size = get_pointer_size(options);
                    auto indexPrefix = std::format(L"+{} [{}] ", stream_hex_dump::to_hex(sizeof(length) * index, pointer_size), index);
                    co_yield variable_symbol_at(walker,
                        options,
                        indexPrefix, 
                        type, 
                        type, 
                        variable_address, 
                        variable_stream, 
                        indexName,
                        visited_pointers,
                        max_symbol_dump_depth,
                        parents);
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

    generator<dump_variable_symbol_data> symbol_data_dumper::pointer_memory_value(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , uint64_t const pointer_value
        , symbol_type_info const& pointer_type
        , mini_dump_memory_stream const& variable_stream
        , std::wstring_view const& name
        , std::unordered_set<uint64_t>& visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<symbol_type_info>& parents) const
    {
        if(auto const data_type_tag = pointer_type.sym_tag(); 
            data_type_tag.has_value() && 
            (max_symbol_dump_depth == 0 || parents.size() < max_symbol_dump_depth))
        {
            parents.push_back(type);
            auto indexName = std::format(L"*{}", name);
            switch(data_type_tag.value())  // NOLINT(clang-diagnostic-switch-enum)
            {
                case sym_tag_enum::UDT:
                    co_yield variable_symbol_at(walker, options, {}, pointer_type, pointer_type, pointer_value, variable_stream, indexName, visited_pointers, max_symbol_dump_depth, parents);
                    break;

                case sym_tag_enum::BaseType:
                    for (auto data : base_type_variable_symbol_at(
                        walker, 
                        options, 
                        pointer_type, 
                        data_type_tag.value(), 
                        pointer_value, variable_stream, 
                        is_pointer_t{ true }, 
                        0, 
                        indexName, 
                        visited_pointers,
                        max_symbol_dump_depth,
                        parents))  // NOLINT(performance-for-range-copy)
                    {
                        co_yield std::move(data);
                    }
                    break;

                case sym_tag_enum::PointerType:
                    co_yield variable_symbol_at(walker, options, {}, pointer_type, pointer_type, pointer_value, variable_stream, indexName, visited_pointers, max_symbol_dump_depth, parents);
                    break;

                default:
                    break;
            }
}
    }


    bool symbol_data_dumper::any_data_at(
        std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , uint64_t const variable_address
        , mini_dump_memory_stream& variable_stream
        , std::optional<symbol_type_info> const& data_type
        , std::optional<sym_tag_enum> const data_type_tag
        , std::unordered_set<uint64_t>& visited_pointers
        )
    {
        // data member, type the data member type and print based on that type
        if (data_type.has_value())
        {
            switch (auto const tag = data_type_tag.value_or(sym_tag_enum::Null); tag)  // NOLINT(clang-diagnostic-switch-enum)
            {
            case sym_tag_enum::UDT:
                return any_variable_symbol_at(data_type.value());
            
            case sym_tag_enum::Enum:
                break;

            case sym_tag_enum::PointerType:
                return any_pointer_variable_symbol_at(os, walker, options, data_type.value(), variable_stream, visited_pointers);

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

    std::optional<symbol_data_dumper::pointer_data_t> symbol_data_dumper::get_pointer(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , mini_dump_memory_stream& variable_stream)
    {
        uint64_t pointer_value{0};
        std::streamsize pointer_size{0};

        if(auto const length = type.length(); length.has_value())
        {
            pointer_size = static_cast<std::streamsize>(length.value());
            switch(pointer_size)
            {
            case 4:
                if(uint32_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                {
                    pointer_value = static_cast<uint64_t>(value);
                }
                break;

            case 8:
                if(uint64_t value; variable_stream.read(&value, sizeof value) == sizeof value)
                {
                    if (fix_wow64_pointer(options, value))
                    {
                        pointer_size = 4;
                    }
                    pointer_value = value;
                }
                break;

            default:
                break;
            }
        }

        if (pointer_value > 0)
        {
            if(auto const pointer_type = type.type(); pointer_type.has_value())
            {
                if(auto const length = pointer_type.value().length(); length.has_value() && length.value() > 0)
                {
                    if(auto pointer_variable_stream = walker.get_process_memory_stream(pointer_value, length.value()); !pointer_variable_stream.eof())
                    {
                        return pointer_data_t
                            {
                                .pointer_value = pointer_value,
                                .pointer_size = pointer_size,
                                .pointer_type = pointer_type.value(),
                                .variable_stream = std::move(pointer_variable_stream)
                            };
                    }
                }
            }
        }

        return std::nullopt;
    }

    bool symbol_data_dumper::any_array_variable_symbol_at(
        stream_stack_dump::mini_dump_memory_walker const& walker
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

    bool symbol_data_dumper::any_base_type_variable_symbol_at(
        stream_stack_dump::mini_dump_memory_walker const& walker
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

    bool symbol_data_dumper::any_variable_symbol_at(symbol_type_info const& type)
    {
        if(auto const length = type.length(); length.has_value())
        {
            return true;
        }

        return false;
    }


    bool symbol_data_dumper::any_pointer_memory_value(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& pointer_type
        , uint64_t const pointer_value
        , mini_dump_memory_stream& variable_stream)
    {
        if(auto const data_type_tag = pointer_type.sym_tag(); data_type_tag.has_value())
        {
            switch(data_type_tag.value())  // NOLINT(clang-diagnostic-switch-enum)
            {
                case sym_tag_enum::UDT:
                    return any_variable_symbol_at(pointer_type);

                case sym_tag_enum::BaseType:
                    return any_base_type_variable_symbol_at(walker, pointer_type, pointer_value, is_pointer_t{ true }, 0);

                case sym_tag_enum::PointerType:
                    if (auto pointer_data = get_pointer(walker, options, pointer_type, variable_stream);
                        pointer_data.has_value())
                    {
                        return true;
                    }
                break;

                default:
                    break;
            }
        }

        return false;
    }

    bool symbol_data_dumper::any_pointer_variable_symbol_at(
        std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_visit_flags::flags const options
        , symbol_type_info const& type
        , mini_dump_memory_stream& variable_stream
        , std::unordered_set<uint64_t>& visited_pointers
        )
    {
        if (auto pointer_data = get_pointer(walker, options, type, variable_stream);
            pointer_data.has_value())
        {
            if (detect_pointer_cycle(os, options, pointer_data.value().pointer_value, visited_pointers))
            {
                return false;
            }

            return any_pointer_memory_value(walker, options, pointer_data.value().pointer_type, pointer_data.value().pointer_value, pointer_data.value().variable_stream);
        }

        return false;
    }

    bool symbol_data_dumper::any_number_variable(
        stream_stack_dump::mini_dump_memory_walker const& walker
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

    bool symbol_data_dumper::any_udt_array(symbol_type_info const& type, size_t const max_size)
    {
        auto const length_data = type.length();
        return length_data.value_or(0) > 0 && max_size > 0;
    }

    void symbol_data_dumper::dump_bitmask(std::wostream& os, symbol_type_info const& type, unsigned long long& bit_mask)
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

    bool symbol_data_dumper::can_dump_tag(sym_tag_enum const tag)
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

    bool symbol_data_dumper::detect_pointer_cycle(
        std::wostream& os
        , symbol_visit_flags::flags const options
        , uint64_t const pointer
        , std::unordered_set<uint64_t>& visited_pointers
        )
    {
        if (pointer == 0 || pointer == 0xffffffffffffffff)
        {
            return false;
        }

        if ((options & symbol_visit_flags::detect_pointer_cycles) == symbol_visit_flags::detect_pointer_cycles)
        {
            if (visited_pointers.contains(pointer))
            {
                os << L' ' << resources::get_pointer_cycle_detected_title();
                return true;
            }
        }
        else if (visited_pointers.contains(pointer))
        {
            os << L' ' << resources::get_pointer_cycle_detected_title();
            return false;
        }

        visited_pointers.insert(pointer);
        return false;
    }

    bool symbol_data_dumper::fix_wow64_pointer(symbol_visit_flags::flags const options, uint64_t& value)
    {
        if ((options & symbol_visit_flags::x86) ==symbol_visit_flags::x86)
        {
            if ((value & 0xffffffff00000000) != 0x0000000000000000)
            {
                value = value & 0x00000000ffffffff;
            }

            return true;
        }

        return false;
    }

    std::streamsize symbol_data_dumper::get_pointer_size(const symbol_visit_flags::flags options)
    {
        if ((options & symbol_visit_flags::x86) ==symbol_visit_flags::x86)
        {
            return sizeof(uint32_t) * 2;
        }

        return sizeof(uint64_t) * 2;
    }
}
