#include "filesystem_utils.h"

#include <locale>

namespace dlg_help_utils::filesystem_utils
{
    bool wildcard_match(std::wstring_view const value, std::wstring_view const match)
    {
        size_t value_index = 0;
        for (size_t match_index = 0; match_index < match.size(); ++match_index)
        {
            switch (match[match_index])
            {
            case '?':
                if (value_index == value.size())
                {
                    return false;
                }

                ++value_index;
                break;

            case '*':
                {
                    if (match_index + 1 == match.size())
                    {
                        return true;
                    }

                    auto const rest_match = match.substr(match_index + 1);
                    for (auto i = value_index; i < value.size(); ++i)
                    {
                        if (wildcard_match(value.substr(i), rest_match))
                        {
                            return true;
                        }
                    }

                    return false;
                }
            default:
                if (toupper(value[value_index]) != toupper(match[match_index]))
                {
                    return false;
                }

                ++value_index;
            }
        }

        return value_index == value.size();
    }
}
