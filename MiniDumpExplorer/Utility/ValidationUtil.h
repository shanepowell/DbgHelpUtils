#pragma once
#include <experimental/generator>

#include "BindingInfo.h"

namespace ValidationUtil
{

    std::vector<Utility::BindingInfo> GetBindingInfoOfSingleElement(winrt::Microsoft::UI::Xaml::FrameworkElement const& element
        , winrt::Windows::Foundation::IInspectable const& inheritedDataContext
        , winrt::Windows::Foundation::IInspectable const& dataItem
        , bool twoWayOnly
        , bool useBlockList);

    std::experimental::generator<winrt::Microsoft::UI::Xaml::DependencyProperty> GetDependencyProperties(winrt::Microsoft::UI::Xaml::FrameworkElement const& element, bool useBlockList);

    template<typename T, typename ...Args>
    bool IsExcludedType(winrt::Microsoft::UI::Xaml::FrameworkElement const& element)
    {
        if(const auto tSourceValue = element.try_as<T>(); tSourceValue)
        {
            return true;
        }

        if constexpr (sizeof...(Args) > 0)
        {
            return IsExcludedType<Args...>(element);
        }

        return false;
    }

    template<typename ...T>
    std::vector<Utility::BindingInfo> GetBindingInfo(winrt::Microsoft::UI::Xaml::FrameworkElement const& element
        , winrt::Windows::Foundation::IInspectable const& dataItem
        , bool const twoWayOnly
        , bool const useBlockList
        , bool const searchChildren)
    {
        std::vector<Utility::BindingInfo> bindingData;

        if (!searchChildren)
        {
            if(IsExcludedType<T...>(element))
            {
                return bindingData;
            }

            auto const dataContext = element.DataContext();
            return GetBindingInfoOfSingleElement(element, dataContext ? dataContext : dataItem, dataItem, twoWayOnly, useBlockList);
        }

        std::vector<winrt::Microsoft::UI::Xaml::DependencyObject> children;
        std::vector<winrt::Windows::Foundation::IInspectable> dataContexts;
        children.push_back(element);
        {
            auto const dataContext = element.DataContext();
            dataContexts.push_back(dataContext ? dataContext : dataItem);
        }

        while (!children.empty())
        {
            auto const& child = children.back();
            children.pop_back();
            auto const& inheritedDataContext = dataContexts.back();
            dataContexts.pop_back();
            winrt::Windows::Foundation::IInspectable dataContext = inheritedDataContext;

            // Skip this particular child element if it is one of the excludedTypes
            if (!IsExcludedType<T...>(element))
            {
                // Add the bindings of the child element and push its children onto the stack of remaining elements to search
                if (auto const childElement = child.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
                    childElement)
                {
                    dataContext = childElement.DataContext();
                    if(!dataContext)
                    {
                        dataContext = inheritedDataContext;
                    }

                    bindingData.append_range(GetBindingInfoOfSingleElement(childElement, inheritedDataContext, dataItem, twoWayOnly, useBlockList));
                }

                auto const childrenCount = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(child);
                for (int32_t childIndex = 0; childIndex < childrenCount; childIndex++)
                {
                    children.push_back(winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetChild(child, childIndex));
                    dataContexts.push_back(dataContext);
                }
            }
        }

        return bindingData;
    }
}
