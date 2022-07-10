#pragma once
#include <string>

#include "windows_setup.h"
#include <WinNls.h>

#include "tagged_bool.h"

namespace locale_formatting
{
    namespace detail
    {
        NUMBERFMTW get_default_number_format_w();
    } // namespace detail

    using floating_point_formatting_t = dlg_help_utils::tagged_bool<struct floating_point_formatting_type>;

    template<typename T>
    std::wstring to_wstring(T value, floating_point_formatting_t const floating_point_formatting = floating_point_formatting_t{false})
    {
        auto v = std::to_wstring(value);
        std::wstring rv;

        auto fmt = detail::get_default_number_format_w();
        if (!floating_point_formatting)
        {
            fmt.NumDigits = 0;
        }

        // ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
        if (auto const size = GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, v.c_str(), &fmt, nullptr, 0); size > 0)
        {
            rv.resize(static_cast<std::wstring::size_type>(size) - 1);
            GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, v.c_str(), &fmt, &rv[0], size);
        }

        return rv;
    }
}
