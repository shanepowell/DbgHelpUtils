#pragma once
#include <format>

#include "units.h"
#include "windows_setup.h"
#include "write_header.h"

namespace dlg_help_utils::stream_hex_dump
{
    namespace details
    {
        template<typename T>
        [[nodiscard]] auto constexpr to_printable_value(T const& value)
        {
            if constexpr (std::is_same_v<T, char> || std::is_same_v<T, int8_t> || std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t> || std::is_same_v<T, wchar_t> || std::is_same_v<T, unsigned char> || std::is_same_v<T, uint8_t>)
            {
                return static_cast<uint16_t>(value);
            }
            else if constexpr (std::is_same_v<T, char32_t> || std::is_enum_v<T>)
            {
                return static_cast<uint32_t>(value);
            }
            else if constexpr (std::is_signed_v<T>)
            {
                return static_cast<std::make_unsigned_t<T>>(value);
            }
            else
            {
                return value;
            }
        }

        template<typename T>
        std::wstring to_hex(T const& value, std::streamsize const width, wchar_t const fill_char = L'0', write_header_t const write_header = write_header_t{true})
        {
            auto result = std::format(L"{0:x}", to_printable_value(value));
            if(auto const result_width = static_cast<std::streamsize>(result.size());
                result_width < width)
            {
                result = std::format(L"{1}{0}", result, std::wstring(static_cast<std::wstring::size_type>(width - result_width), fill_char));
            }

            if(write_header)
            {
                return std::format(L"0x{}", std::move(result));
            }

            return result;
        }

        template <typename UnitType, typename Rep, typename Scale>
        std::wstring to_hex(units::length<UnitType, Rep, Scale> const& value, std::streamsize const width, wchar_t const fill_char = L'0', write_header_t const write_header = write_header_t{true})
        {
            return to_hex(value.count(), width, fill_char, write_header);
        }

        template <typename T>
        std::wstring to_hex(T* value, std::streamsize const width, wchar_t const fill_char = L'0', write_header_t const write_header = write_header_t{true})
        {
            return to_hex(reinterpret_cast<uint64_t>(value), width, fill_char, write_header);
        }

        inline std::wstring to_hex(M128A const& value, std::streamsize const width, wchar_t const fill_char = L'0', write_header_t const write_header = write_header_t{true})
        {
            using namespace std::string_view_literals;
            if (value.High)
            {
                return std::format(L"{0}{1:0>16x}", to_hex(value.High, width > 16 ? (width - 16) : 0, fill_char, write_header), value.Low);
            }

            return to_hex(value.Low, width, fill_char, write_header);
        }
    }

    template <typename T>
    std::wstring to_hex(T value, std::streamsize const width = 0, wchar_t const fill_char = L'0', write_header_t const write_header = write_header_t{true})
    {
        return details::to_hex(value, width, fill_char, write_header);
    }

    template <typename T>
    std::wstring to_hex_raw(T value, std::streamsize const width = 0, wchar_t const fill_char = L'0')
    {
        return details::to_hex(value, width, fill_char, write_header_t{false});
    }

    template <typename T>
    std::wstring to_hex_full(T value, write_header_t const write_header = write_header_t{true})
    {
        return details::to_hex(value, sizeof(T) * 2, L'0', write_header);
    }
}
