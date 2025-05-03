#pragma once
#include <string>

#include "mini_dump_string_stream.h"
#include "stream_stack_dump.h"
#include "stream_utils.h"

namespace dlg_help_utils
{
    using is_x86_target_t = tagged_bool<struct is_x86_target_type>;

    class i_value_type_formatter  // NOLINT(cppcoreguidelines-special-member-functions)
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

        virtual std::wstring format_pointer_value(uint64_t value) const = 0;
        virtual std::wstring format_index_value(size_t index) const = 0;
        virtual std::streamsize pointer_format_width() const = 0;

        bool is_x86_target() const
        {
            constexpr auto x86_pointer_width = get_hex_pointer_width(is_x86_target_t{true});
            return pointer_format_width() == x86_pointer_width;
        }

    protected:
        static constexpr std::streamsize get_hex_pointer_width(is_x86_target_t const is_x86_target)
        {
            if(is_x86_target)
            {
                return sizeof(uint32_t) * 2;
            }

            return sizeof(uint64_t) * 2;
        }
   };
}
