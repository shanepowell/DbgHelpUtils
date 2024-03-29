#include "time_units.h"

#include <sstream>

#include "chrono_unit_convert_to_string.h"
#include "get_label_type_from_labels.h"
#include "size_units.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::time_units
{
    namespace
    {
        std::wstring print_value(time_unit_type const type, auto const us1, auto const us2)
        {
            auto const& str = get_label_strings(type);
            std::wstringstream ss;
            chrono_unit_utilities::convert_to_string(ss, us1, us2, std::get<0>(str), std::get<1>(str));
            return std::move(ss).str();
        }

        template<typename TBsz, typename Tsz>
        std::wstring to_string_internal(TBsz ms)
        {
            return print_value(map_to_time_type<Tsz>::type, ms, TBsz{0});
        }

        template<typename TBsz, typename Tsz, typename Ysz, typename ...Args>
        std::wstring to_string_internal(TBsz ms)
        {
            const auto tb = std::chrono::duration_cast<Tsz>(ms);
            ms -= tb;

            if (tb.count() > 0)
            {
                return print_value(map_to_time_type<Tsz>::type, tb, std::chrono::duration_cast<Ysz>(ms));
            }

            return to_string_internal<TBsz, Ysz, Args...>(ms);
        }
    } // namespace


    std::wstring to_wstring(std::chrono::milliseconds const ms)
    {
        auto years_months_ms = ms;
        const auto yrs = std::chrono::duration_cast<years>(years_months_ms);
        years_months_ms -= yrs;
        const auto mons = std::chrono::duration_cast<months>(years_months_ms);
        years_months_ms -= mons;

        std::wstringstream ss;
        if ((yrs.count() > 0 || mons.count() > 0) && years_months_ms.count() == 0)
        {
            if (yrs.count() > 0)
            {
                const auto& [name, plural_name] = get_label_strings(time_unit_type::year);
                chrono_unit_utilities::convert_to_string(ss, yrs, mons, name, plural_name);
            }
            else
            {
                ss << mons.count() << L" " << get_label_string(time_unit_type::month,
                                                               mons.count() == 1
                                                                   ? string_type::singular
                                                                   : string_type::plural);
            }
        }
        else
        {
            return to_string_internal<std::chrono::milliseconds, weeks, days, std::chrono::hours, std::chrono::minutes, std::chrono::seconds, std::chrono::milliseconds>(ms);
        }

        return ss.str();
    }

    std::wstring const& get_label_string(const time_unit_type time_type, const string_type type)
    {
        auto const& str = get_label_strings(time_type);
        return type == string_type::singular ? std::get<0>(str) : std::get<1>(str);
    }

    std::tuple<std::wstring, std::wstring> const& get_label_strings(const time_unit_type type)
    {
        auto const& type_strings = details::get_type_strings();
        return type_strings.at(type);
    }

    time_unit_type get_label_type(std::wstring_view const label)
    {
        auto const& type_strings = details::get_type_strings();
        if(auto const type = utilities::get_label_type_from_labels(label, type_strings); type.has_value())
        {
            return type.value();
        }

        throw exceptions::wide_runtime_error{std::format(L"Unknown time units type [{}]", label)};
    }
}
