#pragma once
#include <limits>

#ifdef max
#undef max
#endif

namespace dlg_help_utils::range_utils
{
    template <typename T, typename Tl>
    void limit_range(T start_range, T end_range, Tl& length)
    {
        if (start_range > end_range - length)
        {
            length = end_range - start_range;
        }
    }

    template <typename T, typename Tl>
    void limit_range(T start_range, Tl& length)
    {
        limit_range<T, Tl>(start_range, std::numeric_limits<T>::max(), length);
    }

    template <typename T>
    bool range_contains(T start_range, size_t range_length, T start, size_t length)
    {
        return start >= start_range && start + length <= start_range + range_length;
    }

    template <typename T>
    bool range_union(T start_range, size_t range_length, T start, size_t& length)
    {
        if (!range_contains(start_range, range_length, start, 1))
        {
            return false;
        }

        if (start + length >= start_range + range_length)
        {
            length = (start_range + range_length) - start;
        }

        return true;
    }

    template <typename T, typename Tl>
    bool validate_range(T start_range, T end_range, Tl length)
    {
        if (start_range > end_range - length)
        {
            return false;
        }
        return true;
    }

    template <typename T, typename Tl>
    bool validate_range(T start_range, Tl length)
    {
        return validate_range<T, Tl>(start_range, std::numeric_limits<T>::max(), length);
    }
}
