#include "hex_dump.h"

#include <string>

#include "mini_dump_memory_stream.h"
#include "stream_hex_dump.h"

using namespace std;

namespace
{
    size_t hex_string_size_for(size_t value)
    {
        size_t length(1);
        value >>= 4;
        while (value > 0)
        {
            ++length;
            value >>= 4;
        }

        return length;
    }
}

namespace dlg_help_utils::hex_dump
{
    using namespace stream_hex_dump;

    void hex_dump(std::wostream& os, void const* data, size_t const length, size_t const indent, bool const write_header, size_t const bytes_per_row, uint64_t const offset)
    {
        auto const* values = static_cast<uint8_t const*>(data);

        const wstring indent_str(indent, L' ');

        auto const length_hex_size_bytes = static_cast<streamsize>(hex_string_size_for(offset + length));
        auto const bytes_per_row_hex_size_bytes = static_cast<streamsize>(std::max<>(hex_string_size_for(bytes_per_row), static_cast<size_t>(2)));

        if (write_header)
        {
            os << indent_str << wstring(static_cast<size_t>(length_hex_size_bytes) + 4, L' ');

            for (size_t header_index = 0; header_index < bytes_per_row; ++header_index)
            {
                os << to_hex(header_index, bytes_per_row_hex_size_bytes, ' ', false) << L' ';
            }

            os << L" ";

            for (size_t header_index = 0; header_index < bytes_per_row; ++header_index)
            {
                os << to_hex(header_index % 0x10, 0, 0, false);
            }

            os << L'\n';
        }

        for (size_t row = 0; row * bytes_per_row < length; ++row)
        {
            //indent the memory dump
            os << indent_str;

            //write the address of the first byte of this row
            os << to_hex(static_cast<uint64_t>(row * bytes_per_row) + offset, length_hex_size_bytes) << L": ";

            //first write out the hexadecimal values of the individual bytes
            auto const num_bytes = std::min<>(bytes_per_row, length - row * bytes_per_row);
            for (size_t column = 0; column < num_bytes; ++column)
            {
                os << to_hex(static_cast<unsigned int>(values[bytes_per_row * row + column]), bytes_per_row_hex_size_bytes, L'0', false) << L' ';
            }

            //next, the last row may contain less bytes, so we need to pad the output a bit
            if (num_bytes != bytes_per_row)
            {
                auto const gaps = bytes_per_row - num_bytes;
                std::wstring const spaces(gaps * static_cast<size_t>(bytes_per_row_hex_size_bytes + 1), L' ');
                os << spaces;
            }

            //put in a bit of a gap
            os << L' ';

            //and now write the actual bytes themselves
            for (size_t column = 0; column < num_bytes; ++column)
            {
                if (auto const c = values[bytes_per_row * row + column]; isprint(c))
                {
                    os << static_cast<char>(c);
                }
                else
                {
                    os << L'.';
                }
            }
            os << L'\n';
        }
    }

    void hex_dump(std::wostream& os, mini_dump_memory_stream& stream, size_t const length, size_t const indent, bool const write_header, size_t const bytes_per_row, uint64_t const offset)
    {
        const wstring indent_str(indent, L' ');

        auto const length_hex_size_bytes = static_cast<streamsize>(hex_string_size_for(offset + length));
        auto const bytes_per_row_hex_size_bytes = static_cast<streamsize>(std::max<>(hex_string_size_for(bytes_per_row), static_cast<size_t>(2)));

        if (write_header)
        {
            os << indent_str << wstring(static_cast<size_t>(length_hex_size_bytes) + 4, L' ');

            for (size_t header_index = 0; header_index < bytes_per_row; ++header_index)
            {
                os << to_hex(header_index, bytes_per_row_hex_size_bytes, ' ', false) << L' ';
            }

            os << L" ";

            for (size_t header_index = 0; header_index < bytes_per_row; ++header_index)
            {
                os << to_hex(header_index % 0x10, 0, 0, false);
            }

            os << L'\n';
        }

        const auto values = std::make_unique<uint8_t[]>(bytes_per_row);
        for (size_t row = 0; row * bytes_per_row < length; ++row)
        {
            auto const num_bytes = stream.read(values.get(), bytes_per_row);

            //indent the memory dump
            os << indent_str;

            //write the address of the first byte of this row
            os << to_hex(static_cast<uint64_t>(row * bytes_per_row) + offset, length_hex_size_bytes) << L": ";

            //first write out the hexadecimal values of the individual bytes
            for (size_t column = 0; column < num_bytes; ++column)
            {
                os << to_hex(static_cast<unsigned int>(values[column]), bytes_per_row_hex_size_bytes, L'0', false) << L' ';
            }

            //next, the last row may contain less bytes, so we need to pad the output a bit
            if (num_bytes != bytes_per_row)
            {
                auto const gaps = bytes_per_row - num_bytes;
                std::wstring const spaces(gaps * static_cast<size_t>(bytes_per_row_hex_size_bytes + 1), L' ');
                os << spaces;
            }

            //put in a bit of a gap
            os << L' ';

            //and now write the actual bytes themselves
            for (size_t column = 0; column < num_bytes; ++column)
            {
                if (auto const c = values[column]; isprint(c))
                {
                    os << static_cast<char>(c);
                }
                else
                {
                    os << L'.';
                }
            }
            os << L'\n';
        }
    }
}
