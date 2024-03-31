#include "size_units.h"

#include <optional>
#include <sstream>

#include "get_label_type_from_labels.h"
#include "unit_convert_to_string.h"
#include "string_utils.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::size_units
{
    namespace
    {
        std::wstring print_value(size_unit_type const type, auto const us1, auto const us2, print const p)
        {
            auto const& str = get_label_strings(type);
            std::wstringstream ss;
            if(p == print::compact)
            {
                unit_utilities::convert_to_compact_string(ss, us1, us2, std::get<2>(str));
            }
            else
            {
                unit_utilities::convert_to_string(ss, us1, us2, std::get<0>(str), std::get<1>(str));
            }

            return std::move(ss).str();
        }

        template<typename TBsz, typename Tsz>
        std::wstring to_string_internal(TBsz b, print const p)
        {
            return print_value(map_to_size_type<Tsz>::type, b, TBsz{0}, p);
        }

        template<typename TBsz, typename Tsz, typename Ysz, typename ...Args>
        std::wstring to_string_internal(TBsz b, print const p)
        {
            const auto tb = units::length_cast<Tsz>(b);
            b -= tb;

            if (tb.count() > 0)
            {
                return print_value(map_to_size_type<Tsz>::type, tb, units::length_cast<Ysz>(b), p);
            }

            return to_string_internal<TBsz, Ysz, Args...>(b, p);
        }
    }

    namespace base_10
    {
        std::wstring to_wstring(bytes const b, print const p)
        {
            return to_string_internal<bytes, exabytes, petabytes, terabytes, gigabytes, megabytes, kilobytes, bytes>(b, p);
        }

        std::wstring to_bytes_wstring(bytes const b, print const p)
        {
            return print_value(size_unit_type::bytes, b, bytes{0}, p);
        }

        std::wstring to_kilobytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<kilobytes>(b);
            b -= value;
            return print_value(size_unit_type::kilobytes, value, b, p);
        }

        std::wstring to_megabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<megabytes>(b);
            b -= value;
            return print_value(size_unit_type::megabytes, value, units::length_cast<kilobytes>(b), p);
        }

        std::wstring to_gigabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<gigabytes>(b);
            b -= value;
            return print_value(size_unit_type::gigabytes, value, units::length_cast<megabytes>(b), p);
        }

        std::wstring to_terabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<terabytes>(b);
            b -= value;
            return print_value(size_unit_type::terabytes, value, units::length_cast<gigabytes>(b), p);
        }

        std::wstring to_petabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<petabytes>(b);
            b -= value;
            return print_value(size_unit_type::petabytes, value, units::length_cast<terabytes>(b), p);
        }

        std::wstring to_exabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<exabytes>(b);
            b -= value;
            return print_value(size_unit_type::exabytes, value, units::length_cast<petabytes>(b), p);
        }

        bytes from_wstring(std::wstring const& value)
        {
            wchar_t *end_ptr{nullptr};


            auto const units_value = wcstoull(value.c_str(), &end_ptr, 10);
            std::wstring_view label(end_ptr);

            label = string_utils::trim_start(' ', label);

            if(label.empty())
            {
                return to<bytes>(units_value);
            }

            switch(get_label_type(label))
            {
            case size_unit_type::exabytes: return to<exabytes>(units_value);
            case size_unit_type::petabytes: return to<petabytes>(units_value);
            case size_unit_type::terabytes: return to<terabytes>(units_value);
            case size_unit_type::gigabytes: return to<gigabytes>(units_value);
            case size_unit_type::megabytes: return to<megabytes>(units_value);
            case size_unit_type::kilobytes: return to<kilobytes>(units_value);
            case size_unit_type::bytes: return to<bytes>(units_value);
            }

            throw exceptions::wide_runtime_error{std::format(L"Unknown units value [{}]", value)};
        }
    }

    namespace base_16
    {
        std::wstring to_wstring(bytes const b, print const p)
        {
            return to_string_internal<bytes, exabytes, petabytes, terabytes, gigabytes, megabytes, kilobytes, bytes>(b, p);
        }

        std::wstring to_bytes_wstring(bytes const b, print const p)
        {
            return print_value(size_unit_type::bytes, b, bytes{0}, p);
        }

        std::wstring to_kilobytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<kilobytes>(b);
            b -= value;
            return print_value(size_unit_type::kilobytes, value, b, p);
        }

        std::wstring to_megabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<megabytes>(b);
            b -= value;
            return print_value(size_unit_type::megabytes, value, units::length_cast<kilobytes>(b), p);
        }

        std::wstring to_gigabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<gigabytes>(b);
            b -= value;
            return print_value(size_unit_type::gigabytes, value, units::length_cast<megabytes>(b), p);
        }

        std::wstring to_terabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<terabytes>(b);
            b -= value;
            return print_value(size_unit_type::terabytes, value, units::length_cast<gigabytes>(b), p);
        }

        std::wstring to_petabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<petabytes>(b);
            b -= value;
            return print_value(size_unit_type::petabytes, value, units::length_cast<terabytes>(b), p);
        }

        std::wstring to_exabytes_wstring(bytes b, print const p)
        {
            const auto value = units::length_cast<exabytes>(b);
            b -= value;
            return print_value(size_unit_type::exabytes, value, units::length_cast<petabytes>(b), p);
        }

        bytes from_wstring(std::wstring const& value)
        {
            wchar_t *end_ptr{nullptr};
            auto const units_value = wcstoull(value.c_str(), &end_ptr, 10);
            const std::wstring_view label(end_ptr);

            if(label.empty())
            {
                return to<bytes>(units_value);
            }

            switch(get_label_type(label))
            {
            case size_unit_type::exabytes: return to<exabytes>(units_value);
            case size_unit_type::petabytes: return to<petabytes>(units_value);
            case size_unit_type::terabytes: return to<terabytes>(units_value);
            case size_unit_type::gigabytes: return to<gigabytes>(units_value);
            case size_unit_type::megabytes: return to<megabytes>(units_value);
            case size_unit_type::kilobytes: return to<kilobytes>(units_value);
            case size_unit_type::bytes: return to<bytes>(units_value);
            }

            throw exceptions::wide_runtime_error{std::format(L"Unknown units value [{}]", value)};
        }
    }

    std::wstring const& get_label_string(const size_unit_type time_type, const string_type type)
    {
        auto const& str = get_label_strings(time_type);
        switch(type)
        {
        case string_type::singular: return std::get<0>(str);
        case string_type::plural: return std::get<1>(str);
        case string_type::compact: break;
        }
         return std::get<2>(str);
    }

    std::tuple<std::wstring, std::wstring, std::wstring> const& get_label_strings(const size_unit_type type)
    {
        auto const& type_strings = resources::get_type_strings();
        return type_strings.at(type);
    }

    size_unit_type get_label_type(std::wstring_view const label)
    {
        auto const& type_strings = resources::get_type_strings();
        if(auto const type = utilities::get_label_type_from_labels(label, type_strings); type.has_value())
        {
            return type.value();
        }

        throw exceptions::wide_runtime_error{std::format(L"Unknown size units type [{}]", label)};
    }
}
