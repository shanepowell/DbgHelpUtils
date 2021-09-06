#pragma once
#include <cctype>
#include <iomanip>
#include <sstream>
// ReSharper disable once CppUnusedIncludeDirective
#include <type_traits>

#include "mini_dump_memory_stream.h"
#include "stream_hex_dump.h"

namespace dlg_help_utils::print_utils
{
    template<typename T>
    auto to_printable_char(T ch)
    {
        if constexpr (std::is_same_v<T, char> || std::is_same_v<T, wchar_t>)
        {
            return ch;
        }
        else if constexpr (std::is_same_v<T, char8_t>)
        {
            return static_cast<char>(ch);
        }
        else if constexpr (std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>)
        {
            return static_cast<wchar_t>(ch);
        }
    }

    template<typename T>
    bool is_printable_char(T ch)
    {
        if(static_cast<int>(ch) < -1 || static_cast<int>(ch) > 255)
        {
            return false;
        }

        return std::isprint(ch) != 0;
    }

    template<typename T>
    void print_str(std::wostream& os, T const* str, size_t const max_size, bool const stop_at_null)
    {
        for(size_t i = 0; i < max_size; ++i)
        {
            if(stop_at_null && str[i] == NULL)
            {
                return;
            }

            if(is_printable_char(str[i]))
            {
                os << print_utils::to_printable_char(str[i]);
            }
            else
            {
                os << '?';
            }
        }
    }

    template<typename T>
    void print_stream_str(std::wostream& os, mini_dump_memory_stream& stream, size_t const max_size, bool const stop_at_null)
    {
        for(size_t i = 0; i < max_size; ++i)
        {
            T ch;
            if(stream.read(&ch, sizeof T) != sizeof T)
            {
                return;
            }

            if(stop_at_null && ch == NULL)
            {
                return;
            }

            if(is_printable_char(ch))
            {
                os << print_utils::to_printable_char(ch);
            }
            else
            {
                os << '?';
            }
        }
    }

    template<typename T>
    void print_array_str(std::wostream& os, T const* str, size_t const max_size, size_t const indent = 0)
    {
        std::wstring const indent_str(indent, L' ');
        auto null_found = false;
        auto first = true;
        for(size_t i = 0; i < max_size; ++i)
        {
            if(str[i] == NULL)
            {
                if(null_found)
                {
                    // double null found, that's the end of the array
                    return;
                }

                null_found = true;
                first = true;
                os << '\n';
                continue;
            }

            if(first)
            {
                os << indent_str;
                first = false;
            }

            null_found = false;
            if(is_printable_char(str[i]))
            {
                os << print_utils::to_printable_char(str[i]);
            }
            else
            {
                os << '?';
            }
        }
    }

    template<typename T>
    void print_stream_array_str(std::wostream& os, mini_dump_memory_stream& stream, size_t const max_size, size_t const indent = 0)
    {
        std::wstring const indent_str(indent, L' ');
        auto null_found = false;
        auto first = true;
        for(size_t i = 0; i < max_size; ++i)
        {
            T ch;
            if(stream.read(&ch, sizeof T) != sizeof T)
            {
                break;
            }

            if(ch == NULL)
            {
                if(null_found)
                {
                    // double null found, that's the end of the array
                    return;
                }

                null_found = true;
                first = true;
                os << '\n';
                continue;
            }

            if(first)
            {
                os << indent_str;
                first = false;
            }

            null_found = false;
            if(is_printable_char(ch))
            {
                os << print_utils::to_printable_char(ch);
            }
            else
            {
                os << '?';
            }
        }
    }

    template<typename T>
    void print_array_inline(std::wostream& os, T const* values, size_t const max_size, bool const dump_hex = false)
    {
        auto first = true;
        for(size_t i = 0; i < max_size; ++i)
        {
            if(first)
            {
                first = false;
            }
            else
            {
                os << L", ";
            }
            if(dump_hex)
            {
                os << stream_hex_dump::to_hex_full(values[i]);
            }
            else
            {
                os << std::to_wstring(values[i]);
            }
        }
    }

    template<typename T>
    void print_stream_array_inline(std::wostream& os, mini_dump_memory_stream& stream, size_t const max_size, bool const dump_hex = false)
    {
        auto first = true;
        for(size_t i = 0; i < max_size; ++i)
        {
            if(first)
            {
                first = false;
            }
            else
            {
                os << L", ";
            }

            T value;
            if(stream.read(&value, sizeof T) != sizeof T)
            {
                break;
            }

            if(dump_hex)
            {
                os << stream_hex_dump::to_hex_full(value);
            }
            else
            {
                os << std::to_wstring(value);
            }
        }
    }

    template<typename T>
    std::streamsize find_max_hex_width(T const value)
    {
        std::streamsize width;
        if(value <= std::numeric_limits<uint8_t>::max())
        {
            width = 2;
        }
        else if(value <= std::numeric_limits<uint16_t>::max())
        {
            width = 4;
        }
        else if(value <= std::numeric_limits<uint32_t>::max())
        {
            width = 8;
        }
        else
        {
            width = 16;
        }

        return width;
    }

    template<typename T>
    void print_array_lines(std::wostream& os, T const* values, size_t const max_size, size_t const elements_per_line, size_t const element_width, size_t const indent, bool const dump_hex = false)
    {
        std::wstring const indent_str(indent, ' ');

        auto const index_width = find_max_hex_width(max_size - 1);
        auto first = true;
        for(size_t i = 0; i < max_size; ++i)
        {
            if(elements_per_line == 0 || i % elements_per_line == 0)
            {
                if(first)
                {
                    first = false;
                }
                else
                {
                    os << '\n';
                }

                os << indent_str << '[' << stream_hex_dump::to_hex(i, index_width) << L"]: ";
            }

            std::wostringstream ss;
            if(dump_hex)
            {
                ss << stream_hex_dump::to_hex_full(values[i]);
            }
            else
            {
                ss << std::to_wstring(values[i]);
            }

            os << std::setw(static_cast<std::streamsize>(element_width)) << std::setfill(L' ') << std::move(ss).str();
        }
    }

    template<typename T>
    void print_stream_array_lines(std::wostream& os, mini_dump_memory_stream& stream, size_t const max_size, size_t const elements_per_line, size_t const element_width, size_t const indent, bool const dump_hex = false)
    {
        std::wstring const indent_str(indent, ' ');

        auto const index_width = find_max_hex_width(max_size - 1);
        auto first = true;
        for(size_t i = 0; i < max_size; ++i)
        {
            T value;
            if(stream.read(&value, sizeof T) != sizeof T)
            {
                break;
            }

            if(elements_per_line == 0 || i % elements_per_line == 0)
            {
                if(first)
                {
                    first = false;
                }
                else
                {
                    os << '\n';
                }

                os << indent_str << '[' << stream_hex_dump::to_hex(i, index_width) << L"]: ";
            }

            std::wostringstream ss;
            if(dump_hex)
            {
                ss << stream_hex_dump::to_hex_full(value);
            }
            else
            {
                ss << std::to_wstring(value);
            }

            os << std::setw(static_cast<std::streamsize>(element_width)) << std::setfill(L' ') << std::move(ss).str();
        }
    }
}
