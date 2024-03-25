#pragma once

namespace Utility
{
    template<typename T>
    constexpr T winrt_empty_value() noexcept
    {
        if constexpr (std::is_base_of_v<winrt::Windows::Foundation::IUnknown, T>)
        {
            return nullptr;
        }
        else
        {
            return {};
        }
    }
}
