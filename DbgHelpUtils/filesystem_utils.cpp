#include "filesystem_utils.h"

#include <filesystem>
#include <locale>

namespace dlg_help_utils::filesystem_utils
{
    bool wildcard_match(std::wstring_view const value, std::wstring_view const match)
    {
        size_t value_index = 0;
        for (size_t match_index = 0; match_index < match.size(); ++match_index)
        {
            if(value_index == value.size())
            {
                return false;
            }

            switch (match[match_index])
            {
            case '?':
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
                break;
            }
        }

        return value_index == value.size();
    }

    dlg_help_utils::generator<std::wstring> enumerate_files(std::vector<std::wstring> const& paths, std::function<bool(std::wstring)> const& on_error)
    {
        for (auto const& raw_path : paths)
        {
            std::wstring match;
            std::filesystem::path path{raw_path};
            if (exists(path))
            {
                if (is_regular_file(path))
                {
                    co_yield raw_path;
                    continue;
                }

                if (!is_directory(path))
                {
                    if(!on_error(std::format(L"Invalid file path: [{}]", path.wstring())))
                    {
                        co_return;
                    }
                    continue;
                }

                match = L"*";
            }
            else
            {
                match = path.filename().wstring();
                path = path.parent_path();

                if (match.find_first_of(L"*?") == std::wstring::npos)
                {
                    if(!on_error(std::format(L"Invalid wildcard pattern: [{}]", match)))
                    {
                        co_return;
                    }
                    continue;
                }
            }

            if (path.empty())
            {
                path = ".";
            }

            if (!is_directory(path))
            {
                if (!on_error(std::format(L"Invalid directory: [{}]", path.wstring())))
                {
                    co_return;
                }
                continue;
            }

            for (auto const& entry : std::filesystem::directory_iterator(path))
            {
                if (entry.is_regular_file() &&
                    wildcard_match(entry.path().filename().wstring(), match))
                {
                    co_yield entry.path().wstring();
                }
            }
        }
    }
}
