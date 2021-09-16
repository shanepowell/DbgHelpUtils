#pragma once
#include <chrono>
#include <ratio>
#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include <ostream>
#include <unordered_map>

namespace dlg_help_utils::size_units
{
    enum class print
    {
        full,
        compact
    };

    namespace base_10
    {
        using byte  = std::ratio<1>;

        // size_unit TYPEDEFS
        using exabytes = std::chrono::duration<long long, std::exa>;
        using petabytes = std::chrono::duration<long long, std::peta>;
        using terabytes = std::chrono::duration<long long, std::tera>;
        using gigabytes = std::chrono::duration<long long, std::giga>;
        using megabytes = std::chrono::duration<long long, std::mega>;
        using kilobytes = std::chrono::duration<long long, std::kilo>;
        using bytes = std::chrono::duration<long long, byte>;

        std::wstring to_wstring(bytes b, print p = print::compact);

        template <typename T, typename R>
        std::wostream& operator<<(std::wostream& os, std::chrono::duration<T, R> size)
        {
            os << to_wstring(size);
            return os;
        }

        bytes from_wstring(std::wstring const& value);
    }

    namespace base_10_literals
    {
        constexpr base_10::bytes operator "" _b(const unsigned long long val)
        {
            return (base_10::bytes(val));
        }

        constexpr base_10::kilobytes operator "" _kb(const unsigned long long val)
        {
            return (base_10::kilobytes(val));
        }

        constexpr base_10::megabytes operator "" _mb(const unsigned long long val)
        {
            return (base_10::megabytes(val));
        }

        constexpr base_10::gigabytes operator "" _gb(const unsigned long long val)
        {
            return (base_10::gigabytes(val));
        }

        constexpr base_10::terabytes operator "" _tb(const unsigned long long val)
        {
            return (base_10::terabytes(val));
        }

        constexpr base_10::petabytes operator "" _pb(const unsigned long long val)
        {
            return (base_10::petabytes(val));
        }

        constexpr base_10::exabytes operator "" _eb(const unsigned long long val)
        {
            return (base_10::exabytes(val));
        }
    }


    namespace base_16
    {
        // ReSharper disable IdentifierTypo
        using byte  = std::ratio<1>;
        using kilo  = std::ratio<0x400, 1>;
        using mega  = std::ratio<0x100000, 1>;
        using giga  = std::ratio<0x40000000, 1>;
        using tera  = std::ratio<0x10000000000LL, 1>;
        using peta  = std::ratio<0x4000000000000LL, 1>;
        using exa   = std::ratio<0x1000000000000000LL, 1>;
        // ReSharper restore IdentifierTypo

        // size_unit TYPEDEFS
        using exabytes = std::chrono::duration<long long, exa>;
        using petabytes = std::chrono::duration<long long, peta>;
        using terabytes = std::chrono::duration<long long, tera>;
        using gigabytes = std::chrono::duration<long long, giga>;
        using megabytes = std::chrono::duration<long long, mega>;
        using kilobytes = std::chrono::duration<long long, kilo>;
        using bytes = std::chrono::duration<long long, byte>;

        std::wstring to_wstring(bytes b, print p = print::compact);

        template <typename T, typename R>
        std::wostream& operator<<(std::wostream& os, std::chrono::duration<T, R> size)
        {
            os << to_wstring(size);
            return os;
        }

        bytes from_wstring(std::wstring const& value);
    }

    namespace base_16_literals
    {
        constexpr base_16::bytes operator "" _b(const unsigned long long val)
        {
            return (base_16::bytes(val));
        }

        constexpr base_16::kilobytes operator "" _kb(const unsigned long long val)
        {
            return (base_16::kilobytes(val));
        }

        constexpr base_16::megabytes operator "" _mb(const unsigned long long val)
        {
            return (base_16::megabytes(val));
        }

        constexpr base_16::gigabytes operator "" _gb(const unsigned long long val)
        {
            return (base_16::gigabytes(val));
        }

        constexpr base_16::terabytes operator "" _tb(const unsigned long long val)
        {
            return (base_16::terabytes(val));
        }

        constexpr base_16::petabytes operator "" _pb(const unsigned long long val)
        {
            return (base_16::petabytes(val));
        }

        constexpr base_16::exabytes operator "" _eb(const unsigned long long val)
        {
            return (base_16::exabytes(val));
        }
    }


    enum class size_unit_type
    {
        exabytes,
        petabytes,
        terabytes,
        gigabytes,
        megabytes,
        kilobytes,
        bytes
    };

    enum class string_type
    {
        singular,
        plural,
        compact
    };

    std::wstring const& get_label_string(size_unit_type time_type, string_type type);
    std::tuple<std::wstring, std::wstring, std::wstring> const& get_label_strings(size_unit_type type);
    void set_label_strings(std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> user_string_data);
}
