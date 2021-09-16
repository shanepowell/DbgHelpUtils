#pragma once
#include "windows_setup.h"

#include <ostream>
#include <sstream>
#include <iomanip>
#include <format>

namespace dlg_help_utils::stream_hex_dump
{
    namespace details
    {
        template <typename T>
        class hex_converter
        {
        public:
            hex_converter(T value, std::streamsize const width, wchar_t const fill_char, bool const write_header)
                : value_{std::move(value)}
                  , write_header_(write_header)
                  , width_(width)
                  , fill_char_(fill_char)
            {
            }

            template <typename Ts>
            void write_to_stream(Ts& os)
            {
                std::basic_stringstream<typename Ts::char_type, typename Ts::traits_type, std::allocator<typename Ts::char_type>> oss;
                if (write_header_)
                {
                    oss << "0x";
                }

                if (width_ > 0)
                {
                    oss << std::setw(width_) << std::setfill(fill_char_);
                }
                oss << std::hex << to_printable_value() << std::dec;
                os << std::move(oss).str();
            }

            friend std::wostream& operator<<(std::wostream& os, hex_converter<T> raw_value)
            {
                raw_value.write_to_stream(os);
                return os;
            }

            friend std::wostringstream&& operator<<(std::wostringstream&& os, hex_converter<T> raw_value)
            {
                raw_value.write_to_stream(os);
                return std::move(os);
            }

        private:
            [[nodiscard]] auto to_printable_value() const
            {
                if constexpr (std::is_same_v<T, char> || std::is_same_v<T, int8_t> || std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t> || std::is_same_v<T, wchar_t>)
                {
                    return static_cast<int16_t>(value_);
                }
                else if constexpr (std::is_same_v<T, unsigned char> || std::is_same_v<T, uint8_t>)
                {
                    return static_cast<uint16_t>(value_);
                }
                else if constexpr (std::is_same_v<T, char32_t>)
                {
                    return static_cast<int32_t>(value_);
                }
                else
                {
                    return value_;
                }
            }

        private:
            T const value_;
            bool const write_header_;
            std::streamsize const width_;
            wchar_t const fill_char_;
        };

        template <>
        class hex_converter<M128A>
        {
        public:
            hex_converter(M128A value, std::streamsize const width, wchar_t const fill_char, bool const write_header)
                : value_{value}
                  , write_header_(write_header)
                  , width_(width)
                  , fill_char_(fill_char)
            {
            }

            template <typename Ts>
            void write_to_stream(Ts& os)
            {
                if (write_header_)
                {
                    os << L"0x";
                }

                os << std::hex;

                if (value_.High)
                {
                    if (width_ > 16)
                    {
                        os << std::setw(width_ - 16) << std::setfill(fill_char_);
                    }
                    os << value_.High;
                    os << std::setw(16) << std::setfill(L'0');
                }
                else
                {
                    os << std::setw(width_) << std::setfill(fill_char_);
                }

                os << value_.Low << std::dec;
            }

            friend std::wostream& operator<<(std::wostream& os, hex_converter<M128A> raw_value)
            {
                raw_value.write_to_stream(os);
                return os;
            }

            friend std::wostringstream&& operator<<(std::wostringstream&& os, hex_converter<M128A> raw_value)
            {
                raw_value.write_to_stream(os);
                return std::move(os);
            }

        private:
            M128A const value_;
            bool const write_header_;
            std::streamsize const width_;
            wchar_t const fill_char_;
        };
    }

    template <typename T>
    details::hex_converter<T> to_hex(T value, std::streamsize const width = 0, wchar_t const fill_char = L'0',
                                     bool const write_header = true)
    {
        return details::hex_converter<T>{value, width, fill_char, write_header};
    }

    template <typename T>
    details::hex_converter<T> to_hex_raw(T value, std::streamsize const width = 0, wchar_t const fill_char = L'0')
    {
        return details::hex_converter<T>{value, width, fill_char, false};
    }

    template <typename T>
    details::hex_converter<T> to_hex_full(T value, bool const write_header = true)
    {
        return details::hex_converter<T>{value, sizeof(T) * 2, L'0', write_header};
    }
}
