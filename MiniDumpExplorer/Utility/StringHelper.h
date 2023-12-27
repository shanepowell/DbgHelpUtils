#pragma once

#include <string>

namespace StringHelper
{
    auto LastIndexOfAny(std::wstring const& str, auto const first)
    {
        return str.find_last_of(first);
    }

    template<typename ...T>
    auto LastIndexOfAny(std::wstring const& str, auto const first, T... rest)
    {
        auto const last = str.find_last_of(first);
        auto const restLast = LastIndexOfAny(str, rest...);
        if(last == std::wstring::npos)
        {
            return restLast;
        }

        if(restLast == std::wstring::npos)
        {
            return last;
        }

        return std::max(last, restLast);
    }
}