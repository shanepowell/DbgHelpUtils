#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"

#include <array>
#include <xmmintrin.h>

namespace dlg_help_utils::m128a_utils
{
    inline std::array<double, 2> to_doubles(M128A const& value)
    {
        // ReSharper disable CppCStyleCast
        return {*(double const *)&value.High, *(double const *)&value.Low};
        // ReSharper restore CppCStyleCast
    }

    inline std::array<float, 4> to_floats(M128A const& value)
    {
        std::array<float, 4> values;
        auto const& m128 = reinterpret_cast<__m128 const&>(value);
        _mm_storeu_ps(values.data(), m128);
        std::ranges::reverse(values);
        return values;
    }

    inline std::array<int64_t, 2> to_int64s(M128A const& value)
    {
        return { value.High, static_cast<int64_t>(value.Low)};
    }

    inline std::array<int32_t, 4> to_int32s(M128A const& value)
    {
        auto const& m128 = reinterpret_cast<__m128 const&>(value);
        return { m128.m128_i32[3], m128.m128_i32[2], m128.m128_i32[1], m128.m128_i32[0] };
    }

    inline std::array<int16_t, 8> to_int16s(M128A const& value)
    {
        auto const& m128 = reinterpret_cast<__m128 const&>(value);
        return { m128.m128_i16[7], m128.m128_i16[6], m128.m128_i16[5], m128.m128_i16[4], m128.m128_i16[3], m128.m128_i16[2], m128.m128_i16[1], m128.m128_i16[0] };
    }

    inline std::array<int8_t, 16> to_int8s(M128A const& value)
    {
        auto const& m128 = reinterpret_cast<__m128 const&>(value);
        return {
            static_cast<int8_t>(m128.m128_i8[15]),
            static_cast<int8_t>(m128.m128_i8[14]),
            static_cast<int8_t>(m128.m128_i8[13]),
            static_cast<int8_t>(m128.m128_i8[12]),
            static_cast<int8_t>(m128.m128_i8[11]),
            static_cast<int8_t>(m128.m128_i8[10]),
            static_cast<int8_t>(m128.m128_i8[9]),
            static_cast<int8_t>(m128.m128_i8[8]),
            static_cast<int8_t>(m128.m128_i8[7]),
            static_cast<int8_t>(m128.m128_i8[6]),
            static_cast<int8_t>(m128.m128_i8[5]),
            static_cast<int8_t>(m128.m128_i8[4]),
            static_cast<int8_t>(m128.m128_i8[3]),
            static_cast<int8_t>(m128.m128_i8[2]),
            static_cast<int8_t>(m128.m128_i8[1]),
            static_cast<int8_t>(m128.m128_i8[0])
        };
    }

    inline std::array<uint64_t, 2> to_uint64s(M128A const& value)
    {
        return { value.Low, static_cast<uint64_t>(value.High) };
    }

    inline std::array<uint32_t, 4> to_uint32s(M128A const& value)
    {
        auto const& m128 = reinterpret_cast<__m128 const&>(value);
        return { m128.m128_u32[3], m128.m128_u32[2], m128.m128_u32[1], m128.m128_u32[0] };
    }

    inline std::array<uint16_t, 8> to_uint16s(M128A const& value)
    {
        auto const& m128 = reinterpret_cast<__m128 const&>(value);
        return { m128.m128_u16[7], m128.m128_u16[6], m128.m128_u16[5], m128.m128_u16[4], m128.m128_u16[3], m128.m128_u16[2], m128.m128_u16[1], m128.m128_u16[0] };
    }

    inline std::array<uint8_t, 16> to_uint8s(M128A const& value)
    {
        auto const& m128 = reinterpret_cast<__m128 const&>(value);
        return {
            m128.m128_u8[15],
            m128.m128_u8[14],
            m128.m128_u8[13],
            m128.m128_u8[12],
            m128.m128_u8[11],
            m128.m128_u8[10],
            m128.m128_u8[9],
            m128.m128_u8[8],
            m128.m128_u8[7],
            m128.m128_u8[6],
            m128.m128_u8[5],
            m128.m128_u8[4],
            m128.m128_u8[3],
            m128.m128_u8[2],
            m128.m128_u8[1],
            m128.m128_u8[0]
        };
    }

    inline M128A from_doubles(double const v1, double const v2)
    {
        // ReSharper disable CppCStyleCast
        return { *(ULONGLONG const *)&v2, *(LONGLONG const *)&v1 };
        // ReSharper restore CppCStyleCast
    }

    inline M128A from_floats(float const v1, float const v2, float const v3, float const v4)
    {
        auto m128 = _mm_set_ps(v4, v3, v2, v1);
        return reinterpret_cast<M128A const&>(m128);
    }
}
