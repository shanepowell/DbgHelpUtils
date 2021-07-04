#pragma once
#include <chrono>
#include <ratio>
#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include <ostream>
#include <unordered_map>

namespace dlg_help_utils::size_units
{
    namespace base_10
    {
        // size_unit TYPEDEFS
        typedef std::chrono::duration<long long, std::exa> exabytes;
        typedef std::chrono::duration<long long, std::peta> petabytes;
        typedef std::chrono::duration<long long, std::tera> terabytes;
        typedef std::chrono::duration<long long, std::giga> gigabytes;
        typedef std::chrono::duration<long long, std::mega> megabytes;
        typedef std::chrono::duration<long long, std::kilo> kilobytes;
        typedef std::chrono::duration<long long, std::ratio<1>> bytes;


        std::wstring to_wstring(bytes b);

        template <typename T, typename R>
        std::wostream& operator<<(std::wostream& os, std::chrono::duration<T, R> size)
        {
            os << to_wstring(size);
            return os;
        }
    } // namespace base_10

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
        plural
    };

    std::wstring const& get_label_string(size_unit_type time_type, string_type type);
    std::pair<std::wstring, std::wstring> const& get_label_strings(size_unit_type type);
    void set_label_strings(std::unordered_map<size_unit_type, std::pair<std::wstring, std::wstring>> user_string_data);
}
