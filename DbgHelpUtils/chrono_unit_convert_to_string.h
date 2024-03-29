#pragma once
#include <ostream>
#include <chrono>

#include "locale_number_formatting.h"

namespace dlg_help_utils::chrono_unit_utilities
{
    template <typename Us1, typename Us2>
    void convert_to_string(std::wostream& os, Us1 us1, Us2 us2, std::wstring const& us1_name, std::wstring const& us1_plural_name)
    {
        os << us1.count();

        if (us2.count() > 0)
        {
            if (const auto partial = static_cast<unsigned>((static_cast<double>(us2.count()) / static_cast<double>(
                std::chrono::duration_cast<Us2>(Us1{1}).count())) * 100.0); partial > 0)
            {
                auto const& fmt = locale_formatting::get_default_number_format_w();
                os << fmt.lpDecimalSep << partial << L" " << us1_plural_name;
                return;
            }
        }

        os << L" " << (us1.count() <= 1 ? us1_name : us1_plural_name);
    }

    template <typename Us1, typename Us2>
    void convert_to_compact_string(std::wostream& os, Us1 us1, Us2 us2, std::wstring const& us_name)
    {
        os << us1.count();

        if (us2.count() > 0)
        {
            if (const auto partial = static_cast<unsigned>((static_cast<double>(us2.count()) / static_cast<double>(
                std::chrono::duration_cast<Us2>(Us1{1}).count())) * 100.0); partial > 0)
            {
                auto const& fmt = locale_formatting::get_default_number_format_w();
                os << fmt.lpDecimalSep << partial << us_name;
                return;
            }
        }

        os << us_name;
    }
}
