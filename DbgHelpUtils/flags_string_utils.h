#pragma once
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "stream_hex_dump.h"

namespace dlg_help_utils::flags_string_utils
{
    std::wstring get_unknown_options_string();
    std::wstring get_none_options_string();

    template <typename T>
    std::wstring generate_flags_string(T dump_flags, std::map<T, std::wstring_view> const& flag_masks)
    {
        std::wostringstream ss;
        auto first{true};

        for (auto const& [option, title] : flag_masks)
        {
            if (first && option == 0 && dump_flags == option)
            {
                first = false;
                ss << title;
            }
            else if ((dump_flags & option) == option)
            {
                dump_flags &= ~option;
                if (first)
                {
                    first = false;
                }
                else
                {
                    ss << L", ";
                }

                ss << title;
            }
        }

        if (dump_flags > 0)
        {
            if (!first)
            {
                ss << L", ";
            }

            ss << std::format(L"{} [{}]", get_unknown_options_string(), stream_hex_dump::to_hex(dump_flags));
        }
        else if (first)
        {
            ss << get_none_options_string();
        }

        return std::move(ss).str();
    }

    template <typename T>
    std::vector<std::wstring_view> generate_flags_strings(T dump_flags, std::map<T, std::wstring_view> const& flag_masks)
    {
        std::vector<std::wstring_view> rv;

        for (auto const& [option, title] : flag_masks)
        {
            if (option == 0 && dump_flags == option)
            {
                rv.emplace_back(title);
            }
            else if ((dump_flags & option) == option)
            {
                dump_flags &= ~option;
                rv.emplace_back(title);
            }
        }

        return rv;
    }
}
