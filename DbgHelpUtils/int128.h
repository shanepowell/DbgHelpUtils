// ReSharper disable CppClangTidyCertDcl58Cpp
#pragma once

#include <string>
#include <__msvc_int128.hpp>

namespace std
{
    using int128_t = std::_Signed128;
    using uint128_t = std::_Unsigned128;

    std::wstring to_wstring(int128_t value);
    std::wstring to_wstring(uint128_t value);
}

