#pragma once

namespace dlg_help_utils::dbg_help
{
    enum class basic_type : uint8_t
    {
        NoType = 0,
        Void = 1,
        Char = 2,
        WChar = 3,
        Int = 6,
        UInt = 7,
        Float = 8,
        BCD = 9,
        Bool = 10,
        Long = 13,
        ULong = 14,
        Currency = 25,
        Date = 26,
        Variant = 27,
        Complex = 28,
        Bit = 29,
        // ReSharper disable once IdentifierTypo
        BSTR = 30,
        // ReSharper disable once CppInconsistentNaming
        // ReSharper disable once IdentifierTypo
        HResult = 31,
        Char16 = 32,  // char16_t
        Char32 = 33,  // char32_t
        Char8  = 34,  // char8_t
    };
}