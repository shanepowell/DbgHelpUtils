#pragma once
#include <optional>
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>

#include "hash_combine.h"
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
    struct option_mask
    {
        T mask;
        std::optional<T> value;

        [[nodiscard]] std::size_t hash() const
        {
            std::size_t hash{ 0 };
            hash_combine(hash, std::hash<T>{}(mask));
            if (value.has_value())
            {
                hash_combine(hash, std::hash<T>{}(value.value()));
            }
            return hash;
        }

        auto operator<=>(option_mask const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)
    };

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
    auto cast_enum_value(option_mask<T>  value)
    {
        if (value.value.has_value())
        {
            return cast_enum_value(value.value.value());
        }
        return cast_enum_value(value.mask);
    }

    template<typename T>
    bool is_flags_default(T const value, option_mask<T> const& option)
    {
        if (option.value.has_value())
        {
            return cast_enum_value(option.value.value()) == 0 && value == option.value.value();
        }

        return cast_enum_value(option.mask) == 0 && value == option.mask;
    }

    template<typename T1, typename T2>
    bool is_flags_default(T1 const value, T2 const option)
    {

        return cast_enum_value(option) == 0 && value == option;
    }

    template <typename T1, typename T2>
    bool is_flag_option(T1 const value, T2 const option)
    {
        return static_cast<T1>(cast_enum_value(value) & cast_enum_value(option)) == option;
    }

    template <typename T>
    bool is_flag_option(T const value, option_mask<T> const& option)
    {
        if (option.value.has_value())
        {
            return static_cast<T>(cast_enum_value(value) & cast_enum_value(option.mask)) == option.value.value();
        }

        return static_cast<T>(cast_enum_value(value) & cast_enum_value(option.mask)) != 0;
    }

    template <typename T1, typename T2>
    T1 mask_flag_option(T1 const value, T2 const option)
    {
        return static_cast<T1>(cast_enum_value(value) & ~cast_enum_value(option));;
    }

    template <typename T>
    T mask_flag_option(T const value, option_mask<T> const& option)
    {
        return static_cast<T>(cast_enum_value(value) & ~cast_enum_value(option.mask));
    }

    template <typename T1, typename T2>
    T1 add_mask_flag_option(T1 const value, T2 const option)
    {
        return static_cast<T1>(cast_enum_value(value) | cast_enum_value(option));;
    }

    template <typename T>
    T add_mask_flag_option(T const value, option_mask<T> const& option)
    {
        return static_cast<T>(cast_enum_value(value) | cast_enum_value(option.mask));
    }

    template<typename T1, typename T2, typename S>
    std::wstring get_option_title([[maybe_unused]] T1 const value, [[maybe_unused]] T2 const option, S const& title)
    {
        return title;
    }

    template<typename T, typename S>
    std::wstring get_option_title(T const value, option_mask<T> const& option, S const& title)
    {
        if (!option.value.has_value())
        {
            return std::format(L"{} ({})", title, stream_hex_dump::to_hex(cast_enum_value(value) & cast_enum_value(option.mask)));
        }

        return title;
    }

    using mask_used_flags_t = tagged_bool<struct mask_used_flags_type>;

    template <typename T, typename TContainer>
    std::wstring generate_flags_string(T dump_flags, TContainer const& flag_masks, mask_used_flags_t const mask_used_flags)
    {
        std::wostringstream ss;
        auto first{true};
        T used_dump_flags{};

        for (auto const& [option, title] : flag_masks)
        {
            if (first && is_flags_default(dump_flags, option))
            {
                first = false;
                ss << get_option_title(dump_flags, option, title);
            }
            else if (is_flag_option(dump_flags, option))
            {
                if(mask_used_flags)
                {
                    dump_flags = mask_flag_option(dump_flags, option);
                }
                used_dump_flags = add_mask_flag_option(used_dump_flags, option);

                if (auto flag_title = get_option_title(dump_flags, option, title);
                    !flag_title.empty())
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        ss << resources::get_flag_separator_string() << L" ";
                    }

                    ss << std::move(flag_title);
                }
            }
        }

        if (T const left_over_dump_flags = mask_used_flags ? dump_flags : mask_flag_option(dump_flags, used_dump_flags);
            cast_enum_value(left_over_dump_flags) > 0)
        {
            if (!first)
            {
                ss << resources::get_flag_separator_string() << L" ";
            }

            ss << std::format(L"{} [{}]", resources::get_unknown_flags_string(), stream_hex_dump::to_hex(cast_enum_value(left_over_dump_flags)));
        }
        else if (first)
        {
            ss << resources::get_none_options_string();
        }

        return std::move(ss).str();
    }

    template <typename T, typename TContainer>
    std::vector<std::wstring> generate_flags_strings(T dump_flags, TContainer const& flag_masks, mask_used_flags_t const mask_used_flags)
    {
        std::vector<std::wstring> rv;
        T used_dump_flags{};

        for (auto const& [option, title] : flag_masks)
        {
            if (is_flags_default(dump_flags, option))
            {
                rv.emplace_back(get_option_title(dump_flags, option, title));
            }
            else if (is_flag_option(dump_flags, option))
            {
                if(mask_used_flags)
                {
                    dump_flags = mask_flag_option(dump_flags, option);
                }
                used_dump_flags = add_mask_flag_option(used_dump_flags, option);
                if (auto flag_title = get_option_title(dump_flags, option, title);
                    !flag_title.empty())
                {
                    rv.emplace_back(std::move(flag_title));
                }
            }
        }

        if (T const left_over_dump_flags = mask_used_flags ? dump_flags : mask_flag_option(dump_flags, used_dump_flags);
            cast_enum_value(left_over_dump_flags) > 0)
        {
            rv.emplace_back(std::format(L"{} [{}]", resources::get_unknown_flags_string(), stream_hex_dump::to_hex(cast_enum_value(left_over_dump_flags))));
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


template <typename T>
struct std::hash<dlg_help_utils::flags_string_utils::option_mask<T>>  // NOLINT(cert-dcl58-cpp)
{
    std::size_t operator()(dlg_help_utils::flags_string_utils::option_mask<T> const& v) const noexcept
    {
        return v.hash();
    }
};
