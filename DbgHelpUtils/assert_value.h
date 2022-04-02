#pragma once
namespace dlg_help_utils
{
    template <bool AssertionValue, typename ...T>
    struct assert_value
    {
        static_assert(AssertionValue, "Assertion failed <see below for more information>");
        static bool const value = AssertionValue;
    };
}
