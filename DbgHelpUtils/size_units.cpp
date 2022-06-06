#include "size_units.h"

#include <optional>
#include <sstream>

#include "unit_convert_to_string.h"
#include "string_compare.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::size_units
{
    namespace
    {
        std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> populate_default_strings()
        {
            std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> rv;
            rv[size_unit_type::exabytes] = make_tuple(L"exabyte"s, L"exabytes"s, L"eb"s);
            rv[size_unit_type::petabytes] = make_tuple(L"petabyte"s, L"petabytes"s, L"pb"s);
            rv[size_unit_type::terabytes] = make_tuple(L"terabyte"s, L"terabytes"s, L"tb"s);
            rv[size_unit_type::gigabytes] = make_tuple(L"gigabyte"s, L"gigabytes"s, L"gb"s);
            rv[size_unit_type::megabytes] = make_tuple(L"megabyte"s, L"megabytes"s, L"mb"s);
            rv[size_unit_type::kilobytes] = make_tuple(L"kilobyte"s, L"kilobytes"s, L"kb"s);
            rv[size_unit_type::bytes] = make_tuple(L"byte"s, L"bytes"s, L"b"s);
            return rv;
        }

        std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> const g_default_string_data = populate_default_strings();
        std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> g_user_string_data;

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

        std::optional<size_unit_type> get_label_type_from_labels(std::wstring_view const& label, std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> const& labels)
        {
            for(auto const& [type, label_strings] : labels)
            {
                if(auto const& [singular, plural, compact] = label_strings;
                    string_compare::iequals(compact, label) ||
                    string_compare::iequals(singular, label) ||
                    string_compare::iequals(plural, label))
                {
                    return type;
                }
            }

            return std::nullopt;
        }
    }

    namespace base_10
    {
        std::wstring to_wstring(bytes const b, print const p)
        {
            return to_string_internal<bytes, exabytes, petabytes, terabytes, gigabytes, megabytes, kilobytes, bytes>(b, p);
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

    namespace base_16
    {
        std::wstring to_wstring(bytes const b, print const p)
        {
            return to_string_internal<bytes, exabytes, petabytes, terabytes, gigabytes, megabytes, kilobytes, bytes>(b, p);
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
        const auto it = g_user_string_data.find(type);
        if (it == g_user_string_data.end())
        {
            return g_default_string_data.at(type);
        }

        return it->second;
    }

    void set_label_strings(std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> user_string_data)
    {
        g_user_string_data = std::move(user_string_data);
    }

    size_unit_type get_label_type(std::wstring_view const label)
    {
        if(auto const type = get_label_type_from_labels(label, g_user_string_data); type.has_value())
        {
            return type.value();
        }
        if(auto const type = get_label_type_from_labels(label, g_default_string_data); type.has_value())
        {
            return type.value();
        }

        throw exceptions::wide_runtime_error{std::format(L"Unknown units type [{}]", label)};
    }
}
