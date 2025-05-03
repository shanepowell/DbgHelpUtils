#pragma once
#include <string>

#include "mini_dump_string_stream.h"

namespace dlg_help_utils
{
    class  i_value_type_formatter  // NOLINT(cppcoreguidelines-special-member-functions)
    {
    public:
        virtual ~i_value_type_formatter() = default;

        virtual std::wstring format_value(char value) const = 0;
        virtual std::wstring format_value(wchar_t value) const = 0;

        virtual std::wstring format_value(float value) const = 0;
        virtual std::wstring format_value(double value) const = 0;

        virtual std::wstring format_value(mini_dump_string_stream<char> value) const = 0;
        virtual std::wstring format_value(mini_dump_string_stream<wchar_t> value) const = 0;
        virtual std::wstring format_value(mini_dump_string_stream<char8_t> value) const = 0;
        virtual std::wstring format_value(mini_dump_string_stream<char16_t> value) const = 0;
        virtual std::wstring format_value(mini_dump_string_stream<char32_t> value) const = 0;

        virtual std::wstring format_value(int8_t value) const = 0;
        virtual std::wstring format_value(int16_t value) const = 0;
        virtual std::wstring format_value(int32_t value) const = 0;
        virtual std::wstring format_value(int64_t value) const = 0;

        virtual std::wstring format_value(uint8_t value) const = 0;
        virtual std::wstring format_value(uint16_t value) const = 0;
        virtual std::wstring format_value(uint32_t value) const = 0;
        virtual std::wstring format_value(uint64_t value) const = 0;
    };
}
