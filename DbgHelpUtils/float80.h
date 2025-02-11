#pragma once
#include <cstdint>
#include <string>

#include "int128.h"

namespace dlg_help_utils
{
#pragma warning(push)
#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
#pragma pack(push, 1)
    union float80_t
    {
        struct
        {
            uint64_t low;
            uint16_t high;
        };
        struct
        {
            uint64_t significand;
            uint16_t sign:1;
            uint16_t exponent : 15;
        };
    };
#pragma pack(pop)
#pragma warning(pop)

    std::uint128_t float80_to_uint128(float80_t const& value);
    std::wstring to_wstring(float80_t const& value);
}
