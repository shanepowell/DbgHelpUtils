// ReSharper disable CppClangTidyCertDcl58Cpp
#include "int128.h"

#include <algorithm>
#include <sstream>

namespace std
{
    std::wstring to_wstring(int128_t value)
    {
        std::wostringstream ss;
        auto const is_negative = value < 0;

        if (is_negative)
        {
            value = -value;
        }

        while (value > 9)
        {
            ss << static_cast<wchar_t>(L'0' + (value % 10));
            value /= 10;
        }

        if (is_negative)
        {
            ss << L'-';
        }

        auto str = std::move(ss).str();
        std::ranges::reverse(str);
        return str;
    }

    std::wstring to_wstring(uint128_t value)
    {
        std::wostringstream ss;

        while (value > 9)
        {
            ss << static_cast<wchar_t>(L'0' + (value % 10));
            value /= 10;
        }

        auto str = std::move(ss).str();
        std::ranges::reverse(str);
        return str;
    }
}
