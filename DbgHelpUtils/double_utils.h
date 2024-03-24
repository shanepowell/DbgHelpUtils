#pragma once
#include <limits>

namespace double_utils
{
    [[nodiscard]] inline bool are_close(double const value1, double const value2)
    {
        if (value1 == value2)  // NOLINT(clang-diagnostic-float-equal)
        {
            return true;
        }

        // This computes (|value1-value2| / (|value1| + |value2| + 10.0)) < epsilon
        auto const eps = (std::fabs(value1) + std::fabs(value2) + 10.0) * std::numeric_limits<double>::epsilon();
        auto const delta = value1 - value2;
        return -eps < delta && eps > delta;
    }

    [[nodiscard]] inline bool greater_than(double const value1, double const value2)
    {
        return value1 > value2 && !are_close(value1, value2);
    }

    [[nodiscard]] inline bool greater_than_or_close(double const value1, double const value2)
    {
        return value1 > value2 || are_close(value1, value2);
    }

    [[nodiscard]] inline bool less_than(double const value1, double const value2)
    {
        return value1 < value2 && !are_close(value1, value2);
    }

    [[nodiscard]] inline bool less_than_or_close(double const value1, double const value2)
    {
        return value1 < value2 || are_close(value1, value2);
    }

    [[nodiscard]] inline bool is_zero(double const value)
    {
        return std::abs(value) < 10.0 * std::numeric_limits<double>::epsilon();
    }
}
