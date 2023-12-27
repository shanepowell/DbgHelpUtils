#pragma once

namespace Utility
{
    template <typename T>
    std::optional<T> to_optional(winrt::Windows::Foundation::IReference<T> const& value)
    {
        if (value)
        {
            return value.Value();
        }
        return std::nullopt;
    }
}
