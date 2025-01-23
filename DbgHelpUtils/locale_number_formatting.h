#pragma once
#include <format>
#include <string>

#include "windows_setup.h"
#include <WinNls.h>

#include "tagged_bool.h"

namespace locale_formatting
{
    NUMBERFMTW const& get_default_number_format_w();

    using floating_point_formatting_t = dlg_help_utils::tagged_bool<struct floating_point_formatting_type>;
    using floating_point_formatting_scientific_t = dlg_help_utils::tagged_bool<struct floating_point_formatting_scientific_type>;

    template<typename T>
    std::wstring to_wstring(T value, floating_point_formatting_t const floating_point_formatting = floating_point_formatting_t{false}, [[maybe_unused]] floating_point_formatting_scientific_t const floating_point_formatting_scientific = floating_point_formatting_scientific_t{false})
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            if (floating_point_formatting_scientific)
            {
                return std::format(L"{:e}", value);
            }
        }

        auto v = std::to_wstring(value);
        std::wstring rv;

        auto fmt = get_default_number_format_w();
        if (!floating_point_formatting)
        {
            fmt.NumDigits = 0;
        }

        // ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
        if (auto const size = GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, v.c_str(), &fmt, nullptr, 0); size > 0)
        {
            rv.resize(static_cast<std::wstring::size_type>(size) - 1);
            // ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
            GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, v.c_str(), &fmt, rv.data(), size);
        }

        return rv;
    }
}
