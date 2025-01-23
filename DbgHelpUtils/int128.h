// ReSharper disable CppClangTidyCertDcl58Cpp
#pragma once

#include <string>
#include <__msvc_int128.hpp>

struct _M128A;

namespace std
{
    using int128_t = std::_Signed128;
    using uint128_t = std::_Unsigned128;

    int128_t to_int128(_M128A const& value);
    uint128_t to_uint128(_M128A const& value);

    std::wstring to_wstring(int128_t value);
    std::wstring to_wstring(uint128_t value);
}
