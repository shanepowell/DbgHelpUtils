#include "size_units.h"

#include <sstream>

#include "chrono_unit_convert_to_string.h"
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

        std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> const g_default_string_data =
            populate_default_strings();
        std::unordered_map<size_unit_type, std::tuple<std::wstring, std::wstring, std::wstring>> g_user_string_data;
    }

    namespace base_10
    {
        std::wstring to_wstring(bytes b, print const p)
        {
            const auto eb = std::chrono::duration_cast<exabytes>(b);
            b -= eb;
            const auto pb = std::chrono::duration_cast<petabytes>(b);
            b -= pb;
            const auto tb = std::chrono::duration_cast<terabytes>(b);
            b -= tb;
            const auto gb = std::chrono::duration_cast<gigabytes>(b);
            b -= gb;
            const auto mb = std::chrono::duration_cast<megabytes>(b);
            b -= mb;
            const auto kb = std::chrono::duration_cast<kilobytes>(b);
            b -= kb;


            std::wstringstream ss;
            auto const print_value = [p, &ss](size_unit_type const type, auto us1, auto us2)
            {
                auto const& str = get_label_strings(type);
                if(p == print::compact)
                {
                    chrono_unit_utilities::convert_to_compact_string(ss, us1, us2, std::get<2>(str));
                }
                else
                {
                    chrono_unit_utilities::convert_to_string(ss, us1, us2, std::get<0>(str), std::get<1>(str));
                }
            };

            if (eb.count() > 0)
            {
                print_value(size_unit_type::exabytes, eb, pb);
            }
            else if (pb.count() > 0)
            {
                print_value(size_unit_type::petabytes, pb, tb);
            }
            else if (tb.count() > 0)
            {
                print_value(size_unit_type::terabytes, tb, gb);
            }
            else if (gb.count() > 0)
            {
                print_value(size_unit_type::gigabytes, gb, mb);
            }
            else if (mb.count() > 0)
            {
                print_value(size_unit_type::megabytes, mb, kb);
            }
            else if (kb.count() > 0)
            {
                print_value(size_unit_type::kilobytes, kb, b);
            }
            else
            {
                if(p == print::compact)
                {
                    ss << b.count() << get_label_string(size_unit_type::bytes, string_type::compact);
                }
                else
                {
                    ss << b.count() << L" " << get_label_string(size_unit_type::bytes, b.count() == 1 ? string_type::singular : string_type::plural);
                }
            }

            return ss.str();
        }

        bytes from_wstring(std::wstring const& value)
        {
            uint64_t units_value;
            std::wstring units;
            std::wistringstream ss{ value };
            ss >> units_value >> units;
            if (ss.eof())
            {
                if (units.empty() || string_compare::iequals(units, L"b"))
                {
                    return bytes(units_value);
                }

                if (string_compare::iequals(units, L"kb"))
                {
                    return kilobytes(units_value);
                }

                if (string_compare::iequals(units, L"mb"))
                {
                    return megabytes(units_value);
                }

                if (string_compare::iequals(units, L"gb"))
                {
                    return gigabytes(units_value);
                }

                if (string_compare::iequals(units, L"tb"))
                {
                    return terabytes(units_value);
                }

                if (string_compare::iequals(units, L"pb"))
                {
                    return petabytes(units_value);
                }

                if (string_compare::iequals(units, L"eb"))
                {
                    return exabytes(units_value);
                }

                std::wostringstream runtime_error_ss;
                runtime_error_ss << "Unknown units type [" << units << "] for value [" << value << "]";
                throw exceptions::wide_runtime_error{std::move(runtime_error_ss).str()};
            }

            std::wostringstream runtime_error_ss;
            runtime_error_ss << "Unknown units value [" << value << "]";
            throw exceptions::wide_runtime_error{std::move(runtime_error_ss).str()};
        }
    }

    namespace base_16
    {
        std::wstring to_wstring(bytes b, print const p)
        {
            const auto eb = std::chrono::duration_cast<exabytes>(b);
            b -= eb;
            const auto pb = std::chrono::duration_cast<petabytes>(b);
            b -= pb;
            const auto tb = std::chrono::duration_cast<terabytes>(b);
            b -= tb;
            const auto gb = std::chrono::duration_cast<gigabytes>(b);
            b -= gb;
            const auto mb = std::chrono::duration_cast<megabytes>(b);
            b -= mb;
            const auto kb = std::chrono::duration_cast<kilobytes>(b);
            b -= kb;

            std::wstringstream ss;
            auto const print_value = [p, &ss](size_unit_type const type, auto us1, auto us2)
            {
                auto const& str = get_label_strings(type);
                if(p == print::compact)
                {
                    chrono_unit_utilities::convert_to_compact_string(ss, us1, us2, std::get<2>(str));
                }
                else
                {
                    chrono_unit_utilities::convert_to_string(ss, us1, us2, std::get<0>(str), std::get<1>(str));
                }
            };

            if (eb.count() > 0)
            {
                print_value(size_unit_type::exabytes, eb, pb);
            }
            else if (pb.count() > 0)
            {
                print_value(size_unit_type::petabytes, pb, tb);
            }
            else if (tb.count() > 0)
            {
                print_value(size_unit_type::terabytes, tb, gb);
            }
            else if (gb.count() > 0)
            {
                print_value(size_unit_type::gigabytes, gb, mb);
            }
            else if (mb.count() > 0)
            {
                print_value(size_unit_type::megabytes, mb, kb);
            }
            else if (kb.count() > 0)
            {
                print_value(size_unit_type::kilobytes, kb, b);
            }
            else
            {
                if(p == print::compact)
                {
                    ss << b.count() << get_label_string(size_unit_type::bytes, string_type::compact);
                }
                else
                {
                    ss << b.count() << L" " << get_label_string(size_unit_type::bytes, b.count() == 1 ? string_type::singular : string_type::plural);
                }
            }

            return ss.str();
        }

        bytes from_wstring(std::wstring const& value)
        {
            uint64_t units_value;
            std::wstring units;
            std::wistringstream ss{ value };
            ss >> units_value >> units;
            if (ss.eof())
            {
                if (units.empty() || string_compare::iequals(units, L"b"))
                {
                    return bytes(units_value);
                }

                if (string_compare::iequals(units, L"kb"))
                {
                    return kilobytes(units_value);
                }

                if (string_compare::iequals(units, L"mb"))
                {
                    return megabytes(units_value);
                }

                if (string_compare::iequals(units, L"gb"))
                {
                    return gigabytes(units_value);
                }

                if (string_compare::iequals(units, L"tb"))
                {
                    return terabytes(units_value);
                }

                if (string_compare::iequals(units, L"pb"))
                {
                    return petabytes(units_value);
                }

                std::wostringstream runtime_error_ss;
                runtime_error_ss << "Unknown units type [" << units << "] for value [" << value << "]";
                throw exceptions::wide_runtime_error{std::move(runtime_error_ss).str()};
            }

            std::wostringstream runtime_error_ss;
            runtime_error_ss << "Unknown units value [" << value << "]";
            throw exceptions::wide_runtime_error{std::move(runtime_error_ss).str()};
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
}
