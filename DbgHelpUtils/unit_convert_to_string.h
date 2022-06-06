#pragma once
#include <ostream>

#include "units.h"

namespace dlg_help_utils::unit_utilities
{
    template <typename Us1, typename Us2>
    void convert_to_string(std::wostream& os, Us1 us1, Us2 us2, std::wstring const& us1_name, std::wstring const& us1_plural_name)
    {
        using namespace units;
        os << us1.count();

        if (us2.count() > 0)
        {
            if (const auto partial = static_cast<unsigned>((static_cast<double>(us2.count()) / static_cast<double>(
                length_cast<Us2>(Us1{1}).count())) * 100.0); partial > 0)
            {
                os << L"." << partial << L" " << us1_plural_name;
                return;
            }
        }

        os << L" " << (us1.count() <= 1 ? us1_name : us1_plural_name);
    }

    template <typename Us1, typename Us2>
    void convert_to_compact_string(std::wostream& os, Us1 us1, Us2 us2, std::wstring const& us_name)
    {
        using namespace units;
        os << us1.count();

        if (us2.count() > 0)
        {
            if (const auto partial = static_cast<unsigned>((static_cast<double>(us2.count()) / static_cast<double>(
                length_cast<Us2>(Us1{1}).count())) * 100.0); partial > 0)
            {
                os << L"." << partial << us_name;
                return;
            }
        }

        os << us_name;
    }
}
