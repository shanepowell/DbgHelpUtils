#pragma once

namespace Utility
{
    template<typename T>
    void erase_value(T& container, const typename T::value_type& value)
    {
        container.erase(std::remove(container.begin(), container.end(), value), container.end());
    }
}