#include "pch.h"
#include "TypeHelper.h"

// ReSharper disable once CppUnusedIncludeDirective
#include <functional>
#include <ranges>

#include "WinrtDepenencyPropertiesReflectionMapHeaders.h"
#include "DbgHelpUtils/string_utils.h"

using namespace std::string_literals;

namespace TypeHelper
{
    namespace
    {
#include "WinrtDepenencyPropertiesReflectionMap.h"

        winrt::Microsoft::UI::Xaml::DependencyProperty GetDependencyPropertyWithKey(std::map<std::wstring, std::function<winrt::Microsoft::UI::Xaml::DependencyProperty()>> const& objectProperties, std::wstring const& key)
        {
            auto const propertyIt = objectProperties.find(key);

            if(propertyIt == objectProperties.end())
            {
                return nullptr;
            }

            return propertyIt->second();
        }

        winrt::Microsoft::UI::Xaml::DependencyProperty GetDependencyPropertyWithKey(winrt::guid const& id, std::wstring const& key)
        {
            auto const it = InterfaceIdToDependencyProperties.find(id);
            if(it == InterfaceIdToDependencyProperties.end())
            {
                return nullptr;
            }

            return GetDependencyPropertyWithKey(it->second.properties, key);
        }

        // Returns a list of substrings where each one represents a single property within a nested
        // property path which may include indexers.  For example, the string "abc.d[efg][h].ijk"
        // would return the substrings: "abc", "d", "[efg]", "[h]", and "ijk".
        std::experimental::generator<std::wstring_view> SplitPropertyPath(std::wstring_view const& propertyPath)
        {
            if (!propertyPath.empty())
            {
                int startIndex = 0;
                for (int index = 0; index < propertyPath.size(); index++)
                {
                    if (propertyPath[index] == PropertyNameSeparator)
                    {
                        co_yield propertyPath.substr(startIndex, index - startIndex);
                        startIndex = index + 1;
                    }
                    else if (startIndex != index && propertyPath[index] == LeftIndexerToken)
                    {
                        co_yield propertyPath.substr(startIndex, index - startIndex);
                        startIndex = index;
                    }
                    else if (index == propertyPath.size() - 1)
                    {
                        co_yield propertyPath.substr(startIndex);
                    }
                }
            }
        }
    }

    std::wstring GetBindingPropertyName(winrt::Microsoft::UI::Xaml::Data::Binding const& binding)
    {
        auto const path = binding.Path();
        if(!path)
        {
            return {};
        }

        auto const pathString = std::wstring{path.Path()};
        if (pathString.empty())
        {
            return {};
        }

        auto const lastPos = pathString.find_last_of(PropertyNameSeparator);
        if(lastPos == std::wstring::npos)
        {
            return pathString;
        }

        return pathString.substr(lastPos + 1);
    }

    std::experimental::generator<winrt::Microsoft::UI::Xaml::DependencyProperty> GetDependencyProperties(winrt::guid const& id)
    {
        if(auto const it = InterfaceIdToDependencyProperties.find(id);
            it != InterfaceIdToDependencyProperties.end())
        {
            for(const auto& dependencyProperty : it->second.properties | std::views::values)
            {
                co_yield dependencyProperty();
            }
        }
    }

    std::experimental::generator<winrt::Microsoft::UI::Xaml::DependencyProperty> GetDependencyProperties(winrt::Windows::Foundation::IInspectable const& object)
    {
        if(!object)
        {
            co_return;
        }

        for (auto id : winrt::get_interfaces(object))
        {
            for(auto const& property : GetDependencyProperties(id))
            {
                co_yield property;
            }
        }
    }

    winrt::Microsoft::UI::Xaml::DependencyProperty GetDependencyProperty(winrt::Windows::Foundation::IInspectable const& object, std::wstring_view const& name)
    {
        if(!object)
        {
            return nullptr;
        }

        auto const key = dlg_help_utils::string_utils::to_lower(name);
        for (auto id : winrt::get_interfaces(object))
        {
            if(auto property = GetDependencyPropertyWithKey(id, key);
                property)
            {
                return property;
            }
        }

        return nullptr;
    }

    winrt::Microsoft::UI::Xaml::DependencyProperty GetDependencyProperty(winrt::guid const& id, std::wstring_view const& name)
    {
        auto const it = InterfaceIdToDependencyProperties.find(id);
        if(it == InterfaceIdToDependencyProperties.end())
        {
            return nullptr;
        }

        auto const key = dlg_help_utils::string_utils::to_lower(name);
        return GetDependencyPropertyWithKey(it->second.properties, key);
    }

    winrt::Windows::Foundation::IInspectable GetNestedDependencyPropertyValue(winrt::Windows::Foundation::IInspectable object, std::wstring_view const& propertyPath)
    {
        if(!object || propertyPath.empty())
        {
            return nullptr;
        }

        for (auto const& propertyName : SplitPropertyPath(propertyPath))
        {
            if (propertyName.empty())
            {
                continue;
            }

            if (propertyName.size() >= 2 && propertyName[0] == LeftIndexerToken && propertyPath[propertyPath.size() - 1] == RightIndexerToken)
            {
                // indexer...
                auto const index = std::wstring{propertyName.substr(1, propertyName.size() - 2)};

                if(auto const vector = object.try_as<winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Foundation::IInspectable>>();
                    vector)
                {
                    auto const indexValue = std::stoul(index);
                    if(indexValue >= vector.Size())
                    {
                        return nullptr;
                    }

                    object = vector.GetAt(indexValue);
                }
                else if(auto const map = object.try_as<winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, winrt::Windows::Foundation::IInspectable>>();
                                       map)
                {
                    object = map.Lookup(index);
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                auto const dependencyProperty = GetDependencyProperty(object, propertyName);
                if (!dependencyProperty)
                {
                    return nullptr;
                }

                object = GetDependencyPropertyValue(object, dependencyProperty);
            }

            if (!object)
            {
                return nullptr;
            }
        }

        return object;
    }

    void SetDependencyPropertyValue(winrt::Windows::Foundation::IInspectable const& object, winrt::Microsoft::UI::Xaml::DependencyProperty const& dependencyProperty, winrt::Windows::Foundation::IInspectable const& value)
    {
        auto const dependencyObject = object.try_as<winrt::Microsoft::UI::Xaml::DependencyObject>();

        if (!dependencyObject)
        {
            return;
        }

        dependencyObject.SetValue(dependencyProperty, value);
    }

    void SetNestedDependencyPropertyValue(winrt::Windows::Foundation::IInspectable& item, winrt::Windows::Foundation::IInspectable const& newValue, std::wstring_view const& propertyPath)
    {
        if(propertyPath.empty())
        {
            item = newValue;
            return;
        }

        if(!item)
        {
            return;
        }

        auto object = item;
        winrt::Microsoft::UI::Xaml::DependencyProperty objectDependencyProperty{nullptr};
        std::wstring objectDependencyPropertyIndex;

        for (auto const& propertyName : SplitPropertyPath(propertyPath))
        {
            if (propertyName.empty())
            {
                continue;
            }

            if (propertyName.size() >= 2 && propertyName[0] == LeftIndexerToken && propertyPath[propertyPath.size() - 1] == RightIndexerToken)
            {
                // indexer...
                objectDependencyPropertyIndex = std::wstring{propertyName.substr(1, propertyName.size() - 2)};
            }
            else
            {
                if(objectDependencyProperty)
                {
                    object = GetDependencyPropertyValue(object, objectDependencyProperty);

                    if(!objectDependencyPropertyIndex.empty())
                    {
                        if(auto const vector = object.try_as<winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Foundation::IInspectable>>();
                            vector)
                        {
                            auto const indexValue = std::stoul(objectDependencyPropertyIndex);
                            if(indexValue >= vector.Size())
                            {
                                return;
                            }

                            object = vector.GetAt(indexValue);
                        }
                        else if(auto const map = object.try_as<winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, winrt::Windows::Foundation::IInspectable>>();
                                               map)
                        {
                            object = map.Lookup(objectDependencyPropertyIndex);
                        }
                        else
                        {
                            return;
                        }

                        objectDependencyPropertyIndex.clear();
                    }
                }

                if(!object)
                {
                    return;
                }

                objectDependencyProperty = GetDependencyProperty(object, propertyName);
                if (!objectDependencyProperty)
                {
                    return;
                }
            }
        }

        if(objectDependencyProperty)
        {
            if(!objectDependencyPropertyIndex.empty())
            {
                // indexes not allowed in the set value...
                return;
            }

            SetDependencyPropertyValue(object, objectDependencyProperty, newValue);
        }
    }

    void LoadAddApplicationDependencyProperties()
    {
        for(auto const& objectType : InterfaceIdToDependencyProperties | std::views::values | std::views::filter([](auto const& ot) { return ot.type == ObjectType::Application; }))
        {
            for (auto const& property : objectType.properties | std::views::values)
            {
                [[maybe_unused]] auto _ = property();
            }
        }
    }

    std::wstring PrependDefaultMemberName(winrt::Windows::Foundation::IInspectable const& item, std::wstring const& property)
    {
        if (item && !property.empty() && property[0] == LeftIndexerToken)
        {
            // The leaf property name is an indexer, so add the default member name.
            
            if (auto const defaultMember = item.try_as<winrt::MiniDumpExplorer::IDefaultMember>();
                defaultMember)
            {
                if (auto const defaultMemberName = defaultMember.DefaultMememberName();
                    !defaultMemberName.empty())
                {
                    std::wstringstream ss;
                    ss << defaultMemberName << property;
                    return std::move(ss).str();
                }
            }
        }

        return property;
    }

    std::wstring RemoveDefaultMemberName(std::wstring const& property)
    {
        if (!property.empty() && property[property.size() - 1] == RightIndexerToken)
        {
            // The property is an indexer, so remove the default member name.
            if (auto const leftIndexerToken = property.find_first_of(LeftIndexerToken);
                leftIndexerToken != std::wstring::npos)
            {
                return property.substr(leftIndexerToken);
            }
        }

        return property;
    }

    winrt::Windows::Foundation::IInspectable GetDependencyPropertyValue(winrt::Windows::Foundation::IInspectable const& object, winrt::Microsoft::UI::Xaml::DependencyProperty const& dependencyProperty)
    {
        auto const dependencyObject = object.try_as<winrt::Microsoft::UI::Xaml::DependencyObject>();

        if (!dependencyObject)
        {
            return nullptr;
        }

        return dependencyObject.GetValue(dependencyProperty);
    }
}
