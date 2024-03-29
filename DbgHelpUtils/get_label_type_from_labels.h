#pragma once
#include <optional>
#include <unordered_map>

#include "string_compare.h"

namespace dlg_help_utils::utilities
{
    template<size_t Index, typename ...TLabels>
    bool iequals_label(std::wstring_view const& label,  std::tuple<TLabels...> const& labels)
    {
        if constexpr (Index < sizeof...(TLabels))
        {
            if (string_utils::iequals(std::get<Index>(labels), label))
            {
                return true;
            }

            return iequals_label<Index+1, TLabels...>(label, labels);
        }
        else
        {
            return false;
        }
    }

    template<typename T, typename ...TLabels>
    std::optional<T> get_label_type_from_labels(std::wstring_view const& label, std::unordered_map<T, std::tuple<TLabels...>> const& labels)
    {
        for(auto const& [type, label_strings] : labels)
        {
            if(iequals_label<0, TLabels...>(label, label_strings))
            {
                return type;
            }
        }

        return std::nullopt;
    }
}
