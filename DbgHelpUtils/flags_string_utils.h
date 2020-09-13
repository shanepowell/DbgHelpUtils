#pragma once
#include <map>
#include <sstream>
#include <string>

#include "stream_hex_dump.h"

namespace dlg_help_utils::flags_string_utils
{
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

            ss << L"unknown options [" << stream_hex_dump::to_hex(dump_flags) << L"]";
        }
        else if (first)
        {
            ss << L"none";
        }

        return std::move(ss).str();
    }
}
