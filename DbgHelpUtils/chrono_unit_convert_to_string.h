#pragma once
#include <ostream>
#include <chrono>

namespace dlg_help_utils::chrono_unit_utilities
{
    template <typename Us1, typename Us2>
    void convert_to_string(std::wostream& os, Us1 us1, Us2 us2, wchar_t const* us1_name, wchar_t const* us1_plural_name)
    {
        os << us1.count();

        if (us2.count() > 0)
        {
            const auto partial = static_cast<unsigned>((static_cast<double>(us2.count()) / static_cast<double>(
                std::chrono::duration_cast<Us2>(Us1{1}).count())) * 100.0);
            if (partial > 0)
            {
                os << L"." << partial << L" " << us1_plural_name;
                return;
            }
        }

        os << L" " << (us1.count() <= 1 ? us1_name : us1_plural_name);
    }
}
