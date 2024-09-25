#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include <compare>
#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include <ostream>
#include <unordered_map>

#include "units.h"

namespace dlg_help_utils::size_units
{
    enum class print : uint8_t
    {
        full,
        compact
    };

    enum class size_unit_type : uint8_t
    {
        exabytes,
        petabytes,
        terabytes,
        gigabytes,
        megabytes,
        kilobytes,
        bytes
    };


    template<typename T>
    constexpr auto to(uint64_t units)
    {
        using unit_rep = typename T::rep;
        return T{unit_rep{static_cast<unit_rep>(units)}};
    }

    namespace base_10
    {
        struct unit_type_base_10 {};

        template<typename Scale>
        using length = units::length<unit_type_base_10, long long, Scale>;

        using byte  = std::ratio<1>;

        // size_unit TYPEDEFS
        using exabytes = length<std::exa>;
        using petabytes = length<std::peta>;
        using terabytes = length<std::tera>;
        using gigabytes = length<std::giga>;
        using megabytes = length<std::mega>;
        using kilobytes = length<std::kilo>;
        using bytes = length<byte>;

        std::wstring to_wstring(bytes b, print p = print::compact);

        std::wstring to_bytes_wstring(bytes b, print p = print::compact);
        std::wstring to_kilobytes_wstring(bytes b, print p = print::compact);
        std::wstring to_megabytes_wstring(bytes b, print p = print::compact);
        std::wstring to_gigabytes_wstring(bytes b, print p = print::compact);
        std::wstring to_terabytes_wstring(bytes b, print p = print::compact);
        std::wstring to_petabytes_wstring(bytes b, print p = print::compact);
        std::wstring to_exabytes_wstring(bytes b, print p = print::compact);

        template <typename R>
        std::wostream& operator<<(std::wostream& os, length<R> size)
        {
            os << to_wstring(size);
            return os;
        }

        bytes from_wstring(std::wstring const& value);
    }

    namespace base_10_literals
    {
        constexpr base_10::bytes operator""_b(const unsigned long long val)
        {
            return to<base_10::bytes>(val);
        }

        constexpr base_10::kilobytes operator""_kb(const unsigned long long val)
        {
            return to<base_10::kilobytes>(val);
        }

        constexpr base_10::megabytes operator""_mb(const unsigned long long val)
        {
            return to<base_10::megabytes>(val);
        }

        constexpr base_10::gigabytes operator""_gb(const unsigned long long val)
        {
            return to<base_10::gigabytes>(val);
        }

        constexpr base_10::terabytes operator""_tb(const unsigned long long val)
        {
            return to<base_10::terabytes>(val);
        }

        constexpr base_10::petabytes operator""_pb(const unsigned long long val)
        {
            return to<base_10::petabytes>(val);
        }

        constexpr base_10::exabytes operator""_eb(const unsigned long long val)
        {
            return to<base_10::exabytes>(val);
        }
    }


    namespace base_16
    {
        struct unit_type_base_16 {};

        template<typename Scale>
        using length = units::length<unit_type_base_16, long long, Scale>;

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
        using exabytes = length<exa>;
        using petabytes = length<peta>;
        using terabytes = length<tera>;
        using gigabytes = length<giga>;
        using megabytes = length<mega>;
        using kilobytes = length<kilo>;
        using bytes = length<byte>;

        std::wstring to_wstring(bytes b, print p = print::compact);

        std::wstring to_bytes_wstring(bytes b, print p = print::compact);
        std::wstring to_kilobytes_wstring(bytes b, print p = print::compact);
        std::wstring to_megabytes_wstring(bytes b, print p = print::compact);
        std::wstring to_gigabytes_wstring(bytes b, print p = print::compact);
        std::wstring to_terabytes_wstring(bytes b, print p = print::compact);
        std::wstring to_petabytes_wstring(bytes b, print p = print::compact);
        std::wstring to_exabytes_wstring(bytes b, print p = print::compact);

        template <typename R>
        std::wostream& operator<<(std::wostream& os, length<R> size)
        {
            os << to_wstring(size);
            return os;
        }

        bytes from_wstring(std::wstring const& value);
    }

    namespace base_16_literals
    {
        constexpr base_16::bytes operator""_b(const unsigned long long val)
        {
            return to<base_16::bytes>(val);
        }

        constexpr base_16::kilobytes operator""_kb(const unsigned long long val)
        {
            return to<base_16::kilobytes>(val);
        }

        constexpr base_16::megabytes operator""_mb(const unsigned long long val)
        {
            return to<base_16::megabytes>(val);
        }

        constexpr base_16::gigabytes operator""_gb(const unsigned long long val)
        {
            return to<base_16::gigabytes>(val);
        }

        constexpr base_16::terabytes operator""_tb(const unsigned long long val)
        {
            return to<base_16::terabytes>(val);
        }

        constexpr base_16::petabytes operator""_pb(const unsigned long long val)
        {
            return to<base_16::petabytes>(val);
        }

        constexpr base_16::exabytes operator""_eb(const unsigned long long val)
        {
            return to<base_16::exabytes>(val);
        }
    }

    template<typename T> struct map_to_size_type {};
    template<> struct map_to_size_type<base_10::exabytes> { static constexpr size_unit_type type = size_unit_type::exabytes; };
    template<> struct map_to_size_type<base_10::petabytes> { static constexpr size_unit_type type = size_unit_type::petabytes; };
    template<> struct map_to_size_type<base_10::terabytes> { static constexpr size_unit_type type = size_unit_type::terabytes; };
    template<> struct map_to_size_type<base_10::gigabytes> { static constexpr size_unit_type type = size_unit_type::gigabytes; };
    template<> struct map_to_size_type<base_10::megabytes> { static constexpr size_unit_type type = size_unit_type::megabytes; };
    template<> struct map_to_size_type<base_10::kilobytes> { static constexpr size_unit_type type = size_unit_type::kilobytes; };
    template<> struct map_to_size_type<base_10::bytes> { static constexpr size_unit_type type = size_unit_type::bytes; };
    template<> struct map_to_size_type<base_16::exabytes> { static constexpr size_unit_type type = size_unit_type::exabytes; };
    template<> struct map_to_size_type<base_16::petabytes> { static constexpr size_unit_type type = size_unit_type::petabytes; };
    template<> struct map_to_size_type<base_16::terabytes> { static constexpr size_unit_type type = size_unit_type::terabytes; };
    template<> struct map_to_size_type<base_16::gigabytes> { static constexpr size_unit_type type = size_unit_type::gigabytes; };
    template<> struct map_to_size_type<base_16::megabytes> { static constexpr size_unit_type type = size_unit_type::megabytes; };
    template<> struct map_to_size_type<base_16::kilobytes> { static constexpr size_unit_type type = size_unit_type::kilobytes; };
    template<> struct map_to_size_type<base_16::bytes> { static constexpr size_unit_type type = size_unit_type::bytes; };

    enum class string_type : uint8_t
    {
        singular,
        plural,
        compact
    };

    std::wstring const& get_label_string(size_unit_type time_type, string_type type);
    std::tuple<std::wstring, std::wstring, std::wstring> const& get_label_strings(size_unit_type type);
    size_unit_type get_label_type(std::wstring_view label);

    namespace resources
    {
        std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> const& get_type_strings();
    }
}
