#pragma once
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

// ReSharper disable once CppUnusedIncludeDirective
#include <compare>

#include "string_utils.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils
{
    struct range
    {
        uint64_t start;
        uint64_t size;

        auto operator<=>(range const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)
    };

    struct wstring_to_lower
    {
        using Rt = std::wstring;
    };

    struct wstring_to_upper
    {
        using Rt = std::wstring;
    };

    namespace details
    {
        template <typename T, typename Rt>
        Rt string_converter([[maybe_unused]] std::wstring const& value)
        {
            auto constexpr unsupported_string_convert_type = false;
            // ReSharper disable once CppStaticAssertFailure
            static_assert(unsupported_string_convert_type);
            return Rt{};
        }

        template <>
        inline uint32_t string_converter<uint32_t, uint32_t>(std::wstring const& value)
        {
            wchar_t* end;
            auto const rv = std::wcstoul(value.c_str(), &end, 0);

            if (*end != 0)
            {
                throw exceptions::wide_runtime_error(std::format(L"invalid number value [{}]", value));
            }

            return rv;
        }

        template <>
        inline uint64_t string_converter<uint64_t, uint64_t>(std::wstring const& value)
        {
            wchar_t* end;
            auto const rv = std::wcstoull(value.c_str(), &end, 0);

            if (*end != 0)
            {
                throw exceptions::wide_runtime_error(std::format(L"invalid number value [{}]", value));
            }

            return rv;
        }

        template <>
        inline range string_converter<range, range>(std::wstring const& value)
        {
            auto const pos = value.find_first_of(L'-');
            if (pos == std::wstring::npos)
            {
                throw exceptions::wide_runtime_error{std::format(L"Range value: [{}] format incorrect. Expecting <start>-<size> format", value)};
            }

            auto const start = value.substr(0, pos);
            auto const size = value.substr(pos + 1);

            if (start.empty() || size.empty())
            {
                throw exceptions::wide_runtime_error{std::format(L"Range value: [{}] format incorrect. Expecting <start>-<size> format", value)};
            }

            return range{string_converter<uint64_t, uint64_t>(start), string_converter<uint64_t, uint64_t>(size)};
        }

        template <>
        inline std::wstring string_converter<wstring_to_lower, std::wstring>(std::wstring const& value)
        {
            return string_compare::to_lower(value);
        }

        template <>
        inline std::wstring string_converter<wstring_to_upper, std::wstring>(std::wstring const& value)
        {
            return string_compare::to_upper(value);
        }

        template <typename T, typename=void> 
        struct rt_type
        {
            using type = T;
        };

        template <typename T>
        struct rt_type<T, std::void_t<decltype(sizeof(typename T::Rt))>>
        {
            using type = typename T::Rt;
        };

        template<typename T>
        using rt_type_t = typename rt_type<T>::type;
    }

    template <typename T>
    auto vector_to_hash_set(std::vector<std::wstring> const& data)
    {
        using Rt = details::rt_type_t<T>;
        std::set<Rt> rv;
        for (auto const& value : data)
        {
            rv.emplace(details::string_converter<T, Rt>(value));
        }
        return rv;
    }
}
