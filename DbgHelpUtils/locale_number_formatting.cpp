#include "locale_number_formatting.h"

// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
namespace locale_formatting
{
    namespace
    {
        NUMBERFMTW init_default_number_format_w()
        {
            NUMBERFMTW fmt;
            static wchar_t sz_decimal[16] = L"";
            static wchar_t sz_thousand[16] = L"";

            ZeroMemory(&fmt, sizeof fmt);

            GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IDIGITS | LOCALE_RETURN_NUMBER, reinterpret_cast<LPWSTR>(&fmt.NumDigits), sizeof(fmt.NumDigits) / sizeof(wchar_t));
            GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_ILZERO | LOCALE_RETURN_NUMBER, reinterpret_cast<LPWSTR>(&fmt.LeadingZero), sizeof(fmt.LeadingZero) / sizeof(wchar_t));
            wchar_t sz_grouping[32] = L"";
            GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SGROUPING, sz_grouping, ARRAYSIZE(sz_grouping));

            // see https://blogs.msdn.microsoft.com/shawnste/2006/07/17/oddities-of-locale_sgrouping-numbergroupsizes-and-numberfmt/
            for (wchar_t const* ch = sz_grouping; *ch != NULL; ++ch)
            {
                if (isdigit(*ch))
                {
                    if (*ch == L'0')
                    {
                        break;
                    }

                    fmt.Grouping = (fmt.Grouping * 10) + (*ch - L'0');

                    if (*(ch + 1) == NULL)
                    {
                        fmt.Grouping *= 10;
                    }
                }
            }

            GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, sz_decimal, ARRAYSIZE(sz_decimal));
            fmt.lpDecimalSep = sz_decimal;
            GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_STHOUSAND, sz_thousand, ARRAYSIZE(sz_thousand));
            fmt.lpThousandSep = sz_thousand;
            GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_INEGNUMBER | LOCALE_RETURN_NUMBER, reinterpret_cast<LPWSTR>(&fmt.NegativeOrder), sizeof(fmt.NegativeOrder) / sizeof(wchar_t));

            return fmt;
        }
    }  // namespace


    NUMBERFMTW const& get_default_number_format_w()
    {
        static auto fmt = init_default_number_format_w();
        return fmt;
    }
}
// ReSharper restore CppZeroConstantCanBeReplacedWithNullptr
