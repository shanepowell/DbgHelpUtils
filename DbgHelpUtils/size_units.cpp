#include "size_units.h"

#include <sstream>

#include "chrono_unit_convert_to_string.h"
#include "string_compare.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::size_units
{
    namespace
    {
        std::unordered_map<size_unit_type, std::pair<std::wstring, std::wstring>> populate_default_strings()
        {
            std::unordered_map<size_unit_type, std::pair<std::wstring, std::wstring>> rv;
            rv[size_unit_type::exabytes] = make_pair(std::wstring{L"exabyte"}, std::wstring{L"exabytes"});
            rv[size_unit_type::petabytes] = make_pair(std::wstring{L"petabyte"}, std::wstring{L"petabytes"});
            rv[size_unit_type::terabytes] = make_pair(std::wstring{L"terabyte"}, std::wstring{L"terabytes"});
            rv[size_unit_type::gigabytes] = make_pair(std::wstring{L"gigabyte"}, std::wstring{L"gigabytes"});
            rv[size_unit_type::megabytes] = make_pair(std::wstring{L"megabyte"}, std::wstring{L"megabytes"});
            rv[size_unit_type::kilobytes] = make_pair(std::wstring{L"kilobyte"}, std::wstring{L"kilobytes"});
            rv[size_unit_type::bytes] = make_pair(std::wstring{L"byte"}, std::wstring{L"bytes"});
            return rv;
        }

        std::unordered_map<size_unit_type, std::pair<std::wstring, std::wstring>> const g_default_string_data =
            populate_default_strings();
        std::unordered_map<size_unit_type, std::pair<std::wstring, std::wstring>> g_user_string_data;
    }

    namespace base_10
    {
        std::wstring to_wstring(bytes b)
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
            if (eb.count() > 0)
            {
                auto const& str = get_label_strings(size_unit_type::exabytes);
                chrono_unit_utilities::convert_to_string(ss, eb, pb, str.first.c_str(), str.second.c_str());
            }
            else if (pb.count() > 0)
            {
                auto const& str = get_label_strings(size_unit_type::petabytes);
                chrono_unit_utilities::convert_to_string(ss, pb, tb, str.first.c_str(), str.second.c_str());
            }
            else if (tb.count() > 0)
            {
                auto const& str = get_label_strings(size_unit_type::terabytes);
                chrono_unit_utilities::convert_to_string(ss, tb, gb, str.first.c_str(), str.second.c_str());
            }
            else if (gb.count() > 0)
            {
                auto const& str = get_label_strings(size_unit_type::gigabytes);
                chrono_unit_utilities::convert_to_string(ss, gb, mb, str.first.c_str(), str.second.c_str());
            }
            else if (mb.count() > 0)
            {
                auto const& str = get_label_strings(size_unit_type::megabytes);
                chrono_unit_utilities::convert_to_string(ss, mb, kb, str.first.c_str(), str.second.c_str());
            }
            else if (kb.count() > 0)
            {
                auto const& str = get_label_strings(size_unit_type::kilobytes);
                chrono_unit_utilities::convert_to_string(ss, kb, b, str.first.c_str(), str.second.c_str());
            }
            else
            {
                ss << b.count() << L" " << get_label_string(size_unit_type::bytes,
                                                            b.count() == 1
                                                                ? string_type::singular
                                                                : string_type::plural);
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

    std::wstring const& get_label_string(const size_unit_type time_type, const string_type type)
    {
        auto const& str = get_label_strings(time_type);
        return type == string_type::singular ? str.first : str.second;
    }

    std::pair<std::wstring, std::wstring> const& get_label_strings(const size_unit_type type)
    {
        const auto it = g_user_string_data.find(type);
        if (it == g_user_string_data.end())
        {
            return g_default_string_data.at(type);
        }

        return it->second;
    }

    void set_label_strings(std::unordered_map<size_unit_type, std::pair<std::wstring, std::wstring>> user_string_data)
    {
        g_user_string_data = std::move(user_string_data);
    }
}
