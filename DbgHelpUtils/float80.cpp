#include "float80.h"

#include <array>

#include "ryu/ryu_generic_128.h"

namespace dlg_help_utils
{

#define LONG_DOUBLE_MANTISSA_BITS 64
#define LONG_DOUBLE_EXPONENT_BITS 15

    std::uint128_t float80_to_uint128(float80_t const& value)
    {
        return (static_cast<std::uint128_t>(value.high) << 64) | value.low;
    }

    std::wstring to_wstring(float80_t const& value)
    {
        std::array<char, 60> buffer;
        auto v = generic_binary_to_decimal(float80_to_uint128(value), LONG_DOUBLE_MANTISSA_BITS, LONG_DOUBLE_EXPONENT_BITS, true);
        auto const length = generic_to_chars(v, buffer.data());
        return std::wstring{ buffer.data(), buffer.data() + length };
    }
}

