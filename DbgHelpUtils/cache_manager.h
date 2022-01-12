#pragma once
#include <any>
#include <format>
#include <typeindex>
#include <unordered_map>

namespace dlg_help_utils
{

class cache_manager
{
public:
    template<typename T>
    [[nodiscard]] bool has_cache() const
    {
        return cache_.contains(std::type_index{typeid(T)});
    }

    template<typename T>
    [[nodiscard]] T& get_cache()
    {
        auto const key = std::type_index{typeid(T)};
        auto it = cache_.find(key);
        if(it == cache_.end())
        {
            cache_[key] = std::make_any<T>();
            it = cache_.find(key);
        }

        return std::any_cast<T&>(it->second);
    }

    template<typename T>
    [[nodiscard]] T const& get_cache() const
    {
        auto const it = cache_.find(std::type_index{typeid(T)});
        if(it == cache_.end())
        {
            throw std::runtime_error{std::format("No cache found for cache type [{}]", typeid(T).name())};
        }

        return std::any_cast<T const&>(it->second);
    }

private:
    std::unordered_map<std::type_index, std::any> cache_;
};

}