#pragma once
#include <string>

namespace dlg_help_utils
{
    template<typename V>
    std::string join(V const& v,  std::string_view const& delimiter)
    {
        std::string out;
        auto i = std::begin(v);
        auto e = std::end(v);
        if (i != std::end(v))
        {
            out += *i++;
            for (; i != e; ++i) out.append(delimiter).append(*i);
        }
        return out;
    }

    template<typename V>
    std::wstring wjoin(V const& v,  std::wstring_view const& delimiter)
    {
        std::wstring out;
        auto i = std::begin(v);
        auto e = std::end(v);
        if (i != std::end(v))
        {
            out += *i++;
            for (; i != e; ++i) out.append(delimiter).append(*i);
        }
        return out;
    }
}
