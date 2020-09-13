#pragma once
#include <set>
#include <sstream>
#include <string>
#include <vector>
// ReSharper disable once CppUnusedIncludeDirective
#include <compare>

#include "wide_runtime_error.h"

namespace dlg_help_utils
{
    struct range
    {
        uint64_t start;
        uint64_t size;

        auto operator<=>(range const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)
    };

    namespace details
    {
        template <typename T>
        T string_converter([[maybe_unused]] std::wstring const& value)
        {
            auto const unsupported_string_convert_type = false;
            // ReSharper disable once CppStaticAssertFailure
            static_assert(unsupported_string_convert_type);
            return T{};
        }

        template <>
        inline uint32_t string_converter<uint32_t>(std::wstring const& value)
        {
            wchar_t* end;
            auto const rv = std::wcstoul(value.c_str(), &end, 0);

            if (*end != 0)
            {
                std::wostringstream ss;
                ss << L"invalid number value [" << value << L"]";
                throw exceptions::wide_runtime_error(std::move(ss).str());
            }

            return rv;
        }

        template <>
        inline uint64_t string_converter<uint64_t>(std::wstring const& value)
        {
            wchar_t* end;
            auto const rv = std::wcstoull(value.c_str(), &end, 0);

            if (*end != 0)
            {
                std::wostringstream ss;
                ss << L"invalid number value [" << value << L"]";
                throw exceptions::wide_runtime_error(std::move(ss).str());
            }

            return rv;
        }

        template <>
        inline range string_converter<range>(std::wstring const& value)
        {
            auto const pos = value.find_first_of(L'-');
            if (pos == std::wstring::npos)
            {
                std::wostringstream ss;
                ss << "Range value: [" << value << "] format incorrect. Expecting <start>-<size> format";
                throw exceptions::wide_runtime_error{std::move(ss).str()};
            }

            auto const start = value.substr(0, pos);
            auto const size = value.substr(pos + 1);

            if (start.empty() || size.empty())
            {
                std::wostringstream ss;
                ss << "Range value: [" << value << "] format incorrect. Expecting <start>-<size> format";
                throw exceptions::wide_runtime_error{std::move(ss).str()};
            }

            return range{string_converter<uint64_t>(start), string_converter<uint64_t>(size)};
        }
    }

    template <typename T>
    std::set<T> vector_to_hash_set(std::vector<std::wstring> const& data)
    {
        std::set<T> rv;
        for (auto const& value : data)
        {
            rv.emplace(details::string_converter<T>(value));
        }

        return rv;
    }
}
