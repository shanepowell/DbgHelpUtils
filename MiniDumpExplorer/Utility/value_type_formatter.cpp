#include "pch.h"
#include "value_type_formatter.h"

#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/print_utils.h"

#include "Helpers/GlobalOptions.h"

using namespace dlg_help_utils;

namespace 
{
    template<typename T>
    std::wstring format_char_value_impl(T const value)
    {
        switch(GlobalOptions::Options().NumberDisplayFormat())
        {
        default:
        case NumberDisplayFormatType::Hexadecimal:
            return stream_hex_dump::to_hex(value);

        case NumberDisplayFormatType::Decimal:
            return std::format(L"{}", print_utils::to_printable_char(value));
        }
    }

    template<typename T>
    std::wstring format_number_value_impl(T const value)
    {
        switch(GlobalOptions::Options().NumberDisplayFormat())
        {
        default:
        case NumberDisplayFormatType::Hexadecimal:
            return stream_hex_dump::to_hex(value);

        case NumberDisplayFormatType::Decimal:
            return locale_formatting::to_wstring(value);
        }
    }
}

std::wstring value_type_formatter::format_value(char const value) const
{
    return format_char_value_impl(value);
}

std::wstring value_type_formatter::format_value(wchar_t const value) const
{
    return format_char_value_impl(value);
}

std::wstring value_type_formatter::format_value(float const value) const
{
    return std::to_wstring(value);
}

std::wstring value_type_formatter::format_value(double const value) const
{
    return std::to_wstring(value);
}

std::wstring value_type_formatter::format_value(std::string_view const value) const
{
    return print_utils::to_c_string(value);
}

std::wstring value_type_formatter::format_value(std::wstring_view const value) const
{
    return print_utils::to_c_string(value);
}

std::wstring value_type_formatter::format_value(std::basic_string_view<char8_t> const value) const
{
    return print_utils::to_c_string(value);
}

std::wstring value_type_formatter::format_value(std::basic_string_view<char16_t> const value) const
{
    return print_utils::to_c_string(value);
}

std::wstring value_type_formatter::format_value(std::basic_string_view<char32_t> const value) const
{
    return print_utils::to_c_string(value);
}

std::wstring value_type_formatter::format_value(int8_t const value) const
{
    return format_number_value_impl(value);
}

std::wstring value_type_formatter::format_value(int16_t const value) const
{
    return format_number_value_impl(value);
}

std::wstring value_type_formatter::format_value(int32_t const value) const
{
    return format_number_value_impl(value);
}

std::wstring value_type_formatter::format_value(int64_t const value) const
{
    return format_number_value_impl(value);
}

std::wstring value_type_formatter::format_value(uint8_t const value) const
{
    return format_number_value_impl(value);
}

std::wstring value_type_formatter::format_value(uint16_t const value) const
{
    return format_number_value_impl(value);
}

std::wstring value_type_formatter::format_value(uint32_t const value) const
{
    return format_number_value_impl(value);
}

std::wstring value_type_formatter::format_value(uint64_t const value) const
{
    return format_number_value_impl(value);
}
