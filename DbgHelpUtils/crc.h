#pragma once
#include <array>
#include <cstdint>

namespace dlg_help_utils::crc
{
    namespace details
    {
        constexpr auto make_crc_table()
        {
            std::array<uint32_t, 256> crc_table{};

            // terms of polynomial defining this crc (except x^32):
            const uint8_t p[] = {0, 1, 2, 4, 5, 7, 8, 10, 11, 12, 16, 22, 23, 26};

            // ReSharper disable once CommentTypo
            // make exclusive-or pattern from polynomial (0xedb88320L)
            uint32_t poly = 0L;

            for (auto const n : p)
            {
                poly |= 1L << (31 - n);
            }

            for (uint32_t n = 0; n < crc_table.size(); n++)
            {
                auto c = n;
                for (uint32_t k = 0; k < 8; k++)
                {
                    c = c & 1 ? poly ^ (c >> 1) : c >> 1;
                }
#pragma warning(push)
#pragma warning(disable : 28020)
                crc_table[n] = c; //NOLINT
#pragma warning(pop)
            }

            return crc_table;
        }

        static auto constexpr crc_table = make_crc_table();
    }

    constexpr uint32_t crc32(void const* data, size_t const length, uint32_t crc = ~0UL)
    {
        auto const* p = static_cast<uint8_t const*>(data);
        for (size_t i = 0; i < length; ++i)
        {
            crc = details::crc_table[(crc ^ p[i]) & 0xFF] ^ (crc >> 8);
        }

        return crc;
    }

    template <typename T, size_t N>
    constexpr uint32_t crc32(std::array<T, N> const& data, uint32_t const crc = ~0UL)
    {
        return crc32(static_cast<void const*>(data.data()), data.size() * sizeof(T), crc);
    }

    template <typename T, size_t N>
    constexpr uint32_t crc32(T data[N], uint32_t const crc = ~0UL)
    {
        return crc32(static_cast<void const*>(data), N * sizeof(T), crc);
    }
}
