#pragma once
#include <memory>

namespace dlg_help_utils
{
    template <typename S, typename T>
    void hash_combine(S& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
}

