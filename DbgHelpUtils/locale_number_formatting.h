#pragma once
#include <string>

#include "windows_setup.h"
#include <WinNls.h>

namespace locale_formatting
{
    namespace detail
    {
        NUMBERFMTW get_default_number_format_w();
    } // namespace detail

    template<typename T>
    std::wstring to_wstring(T value, const bool floating_point_formatting = false)
    {
        auto v = std::to_wstring(value);
        std::wstring rv;

        auto fmt = detail::get_default_number_format_w();
        if (!floating_point_formatting)
        {
            fmt.NumDigits = 0;
        }

        // ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
        if (auto size = GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, v.c_str(), &fmt, nullptr, 0); size > 0)
        {
            rv.resize(size - 1);
            GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, v.c_str(), &fmt, &rv[0], size);
        }

        return rv;
    }
}
