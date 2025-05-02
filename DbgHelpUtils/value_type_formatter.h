#pragma once
#include <string>

namespace dlg_help_utils::symbol_type_utils
{
    class  i_value_type_formatter  // NOLINT(cppcoreguidelines-special-member-functions)
    {
    public:
        virtual ~i_value_type_formatter() = default;

        virtual std::wstring format_value(char value) const = 0;
        virtual std::wstring format_value(wchar_t value) const = 0;

        virtual std::wstring format_value(float value) const = 0;
        virtual std::wstring format_value(double value) const = 0;

        virtual std::wstring format_value(std::string_view value) const = 0;
        virtual std::wstring format_value(std::wstring_view value) const = 0;
        virtual std::wstring format_value(std::basic_string_view<char8_t> value) const = 0;
        virtual std::wstring format_value(std::basic_string_view<char16_t> value) const = 0;
        virtual std::wstring format_value(std::basic_string_view<char32_t> value) const = 0;

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
