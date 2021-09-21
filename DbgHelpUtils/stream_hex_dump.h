#pragma once
#include <chrono>
#include <format>

#include "windows_setup.h"

namespace dlg_help_utils::stream_hex_dump
{
    namespace details
    {
        template<typename T>
        [[nodiscard]] auto to_printable_value(T const& value)
        {
            if constexpr (std::is_same_v<T, char> || std::is_same_v<T, int8_t> || std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t> || std::is_same_v<T, wchar_t>)
            {
                return static_cast<int16_t>(value);
            }
            else if constexpr (std::is_same_v<T, unsigned char> || std::is_same_v<T, uint8_t>)
            {
                return static_cast<uint16_t>(value);
            }
            else if constexpr (std::is_same_v<T, char32_t>)
            {
                return static_cast<int32_t>(value);
            }
            else if constexpr (std::is_enum_v<T>)
            {
                return static_cast<int32_t>(value);
            }
            else
            {
                return value;
            }
        }

        template<typename T>
        std::wstring to_hex(T const& value, std::streamsize const width, wchar_t const fill_char, bool const write_header)
        {
            using namespace std::string_view_literals;
            auto const spec = std::format(L"{0}{{0:{2}>{1}x}}", write_header ? L"0x"sv : L""sv, width, fill_char);
            return std::format(spec, to_printable_value(value));
        }

        template <typename Rep, typename Period>
        std::wstring to_hex(std::chrono::duration<Rep, Period> const& value, std::streamsize const width, wchar_t const fill_char, bool const write_header)
        {
            return to_hex(value.count(), width, fill_char, write_header);
        }

        template <typename T>
        std::wstring to_hex(T* value, std::streamsize const width, wchar_t const fill_char, bool const write_header)
        {
            return to_hex(reinterpret_cast<uint64_t>(value), width, fill_char, write_header);
        }

        inline std::wstring to_hex(M128A const& value, std::streamsize const width, wchar_t const fill_char, bool const write_header)
        {
            using namespace std::string_view_literals;
            if (value.High)
            {
                auto const spec = std::format(L"{0}{{0:{2}>{1}x}}{{1:{2}>16x}}", write_header ? L"0x"sv : L""sv, width > 16 ? (width - 16) : 0, fill_char);
                return std::format(spec, value.High, value.Low);
            }

            return to_hex(value.Low, width, fill_char, write_header);
        }
    }

    template <typename T>
    std::wstring to_hex(T value, std::streamsize const width = 0, wchar_t const fill_char = L'0', bool const write_header = true)
    {
        return details::to_hex(value, width, fill_char, write_header);
    }

    template <typename T>
    std::wstring to_hex_raw(T value, std::streamsize const width = 0, wchar_t const fill_char = L'0')
    {
        return details::to_hex(value, width, fill_char, false);
    }

    template <typename T>
    std::wstring to_hex_full(T value, bool const write_header = true)
    {
        return details::to_hex(value, sizeof(T) * 2, L'0', write_header);
    }
}
