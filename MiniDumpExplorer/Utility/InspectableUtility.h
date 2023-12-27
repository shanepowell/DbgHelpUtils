#pragma once

#include <optional>

namespace InspectableUtility
{
    template<typename T, typename ...Args>
    auto ProcessValueFromInspectable(auto const& func, winrt::Windows::Foundation::IInspectable const& value, auto defaultValue)
    {
        if(const auto tSourceValue = value.try_as<T>(); tSourceValue.has_value())
        {
            return func(tSourceValue.value());
        }

        if constexpr (sizeof...(Args) > 0)
        {
            return ProcessValueFromInspectable<Args...>(func, value, defaultValue);
        }
        else
        {
            return defaultValue;
        }
    }

    template<typename T, typename ...Args>
    bool IsInspectable(winrt::Windows::Foundation::IInspectable const& value)
    {
        if(const auto tSourceValue = value.try_as<T>(); tSourceValue)
        {
            return true;
        }

        if constexpr (sizeof...(Args) > 0)
        {
            return IsInspectable<Args...>(value);
        }
        else
        {
            return false;
        }
    }

    template<typename T, typename TSource, typename ...Args>
    std::optional<T> ConvertTo(winrt::Windows::Foundation::IInspectable const& value)
    {
        if(const auto tSourceValue = value.try_as<TSource>(); tSourceValue.has_value())
        {
            return static_cast<T>(tSourceValue.value());
        }

        if constexpr (sizeof...(Args) > 0)
        {
            return ConvertTo<T, Args...>(value);
        }
        else
        {
            return std::nullopt;
        }
    }

    template<typename T>
    bool Equals(winrt::Windows::Foundation::IInspectable const& value1, winrt::Windows::Foundation::IInspectable const& value2)
    {
        const auto tValue1 = value1.try_as<T>();
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto tValue2 = value2.try_as<T>();
        if(tValue1.has_value() && tValue2.has_value())
        {
            return tValue1.value() == tValue2.value();
        }

        return false;
    }

    inline bool EqualsIntegral(winrt::Windows::Foundation::IInspectable const& value1, winrt::Windows::Foundation::IInspectable const& value2)
    {
        const auto intValue1 = ConvertTo<uint64_t, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>(value1);
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto intValue2 = ConvertTo<uint64_t, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>(value2);
        if(intValue1.has_value() && intValue2.has_value())
        {
            return intValue1.value() == intValue2.value();
        }

        return false;
    }

    inline bool EqualsFloat(winrt::Windows::Foundation::IInspectable const& value1, winrt::Windows::Foundation::IInspectable const& value2)
    {
        const auto floatValue1 = ConvertTo<double, float, double>(value1);
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto floatValue2 = ConvertTo<double, float, double>(value2);
        if(floatValue1.has_value() && floatValue2.has_value())
        {
            return floatValue1.value() == floatValue2.value();
        }

        return false;
    }

    template<typename T, typename... Args>
    std::optional<bool> IsCollectionEmpty(winrt::Windows::Foundation::IInspectable const& value)
    {
        if (auto const valEnumerable = value.try_as<winrt::Windows::Foundation::Collections::IIterable<T>>(); valEnumerable != nullptr)
        {
            return std::begin(valEnumerable) == std::end(valEnumerable);
        }
        if constexpr (sizeof...(Args) > 0)
        {
            return IsCollectionEmpty<Args...>(value);
        }
        else
        {
            return std::nullopt;
        }
    }
}