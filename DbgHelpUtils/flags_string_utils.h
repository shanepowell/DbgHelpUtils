#pragma once
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>

#include "stream_hex_dump.h"

namespace dlg_help_utils::flags_string_utils
{
    namespace resources
    {
        std::wstring get_unknown_flags_string();
        std::wstring get_flag_separator_string();
        std::wstring get_unknown_enum_string();
        std::wstring get_none_options_string();
    }

    template <typename T>
    auto cast_enum_value(T value)
    {
        if constexpr (std::is_enum_v<T>)
        {
            return static_cast<std::underlying_type_t<T>>(value);
        }
        else
        {
            return value;
        }
    }

    template <typename T>
    bool is_flag_option(T const value, T const option)
    {
        return static_cast<T>(cast_enum_value(value) & cast_enum_value(option)) == option;
    }

    template <typename T>
    T mask_flag_option(T const value, T const option)
    {
        return static_cast<T>(cast_enum_value(value) & ~cast_enum_value(option));;
    }


    template <typename T, typename TContainer>
    std::wstring generate_flags_string(T dump_flags, TContainer const& flag_masks)
    {
        std::wostringstream ss;
        auto first{true};

        for (auto const& [option, title] : flag_masks)
        {
            if (first && cast_enum_value(option) == 0 && dump_flags == option)
            {
                first = false;
                ss << title;
            }
            else if (is_flag_option(dump_flags, option))
            {
                dump_flags = mask_flag_option(dump_flags, option);
                if (first)
                {
                    first = false;
                }
                else
                {
                    ss << resources::get_flag_separator_string() << L" ";
                }

                ss << title;
            }
        }

        if (cast_enum_value(dump_flags) > 0)
        {
            if (!first)
            {
                ss << resources::get_flag_separator_string() << L" ";
            }

            ss << std::format(L"{} [{}]", resources::get_unknown_flags_string(), stream_hex_dump::to_hex(cast_enum_value(dump_flags)));
        }
        else if (first)
        {
            ss << resources::get_none_options_string();
        }

        return std::move(ss).str();
    }

    template <typename T, typename TContainer>
    std::vector<std::wstring> generate_flags_strings(T dump_flags, TContainer const& flag_masks)
    {
        std::vector<std::wstring> rv;

        for (auto const& [option, title] : flag_masks)
        {
            if (cast_enum_value(option) == 0 && dump_flags == option)
            {
                rv.emplace_back(title);
            }
            else if (is_flag_option(dump_flags, option))
            {
                dump_flags = mask_flag_option(dump_flags, option);
                rv.emplace_back(title);
            }
        }

        if (cast_enum_value(dump_flags) > 0)
        {
            rv.emplace_back(std::format(L"{} [{}]", resources::get_unknown_flags_string(), stream_hex_dump::to_hex(cast_enum_value(dump_flags))));
        }

        return rv;
    }

    template <typename T>
    std::wstring generate_enum_string(T enum_value, std::unordered_map<T, std::wstring> const& enum_values)
    {
        if(auto const it = enum_values.find(enum_value); it != enum_values.end())
        {
            return it->second;
        }

        if constexpr (std::is_integral_v<T> || std::is_enum_v<T>)
        {
            return std::format(L"{} [{}]", resources::get_unknown_enum_string(), stream_hex_dump::to_hex(cast_enum_value(enum_value)));
        }
        else
        {
            using namespace std;
            return std::format(L"{} [{}]", resources::get_unknown_enum_string(), to_wstring(enum_value));
        }
    }
}
