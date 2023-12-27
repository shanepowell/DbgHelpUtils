#include "pch.h"
#include "ValidationUtil.h"

#include "Helpers/TypeHelper.h"
#include "Triggers/ControlSizeTrigger.h"

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Windows::Foundation;

namespace ValidationUtil
{
    std::vector<Utility::BindingInfo> GetBindingInfoOfSingleElement(FrameworkElement const& element
        , IInspectable const& inheritedDataContext
        , IInspectable const& dataItem
        , bool const twoWayOnly
        , bool const useBlockList)
    {
        // Now see which of the possible dependency properties are being used
        std::vector<Utility::BindingInfo> bindingData;
        for(auto const bindingTarget : GetDependencyProperties(element, useBlockList))
        {
            // We add bindings according to the same conditions as BindingGroups:
            //    Element.Binding.Mode == TwoWay
            //    Element.Binding.Source == null
            //    DataItem == ContextElement.DataContext where:
            //      If Element is ContentPresenter and TargetProperty is Content, ContextElement = Element.Parent
            //      Else if TargetProperty is DomainContext, ContextElement = Element.Parent
            //      Else ContextElement = Element
            
            if (auto const bindingExpression = element.GetBindingExpression(bindingTarget);
                bindingExpression &&
                bindingExpression.ParentBinding() &&
                (!twoWayOnly || bindingExpression.ParentBinding().Mode() == Data::BindingMode::TwoWay) &&
                !bindingExpression.ParentBinding().Source())
            {
                IInspectable dataContext;
                if (auto const contentPresenter = element.try_as<Controls::ContentPresenter>();
                    bindingTarget == FrameworkElement::DataContextProperty()
                    || (contentPresenter && bindingTarget == contentPresenter.ContentProperty()))
                {
                    dataContext = inheritedDataContext;
                }
                else
                {
                    dataContext = element.DataContext();
                    if(!dataContext)
                    {
                        dataContext = inheritedDataContext;
                    }
                }

                if (dataItem == dataContext)
                {
                    bindingData.push_back(Utility::BindingInfo{bindingExpression, bindingTarget, element});
                }
            }
        }

        return bindingData;
    }

    std::experimental::generator<DependencyProperty> GetDependencyProperties(FrameworkElement const& element, bool const useBlockList)
    {
        if(useBlockList &&
            (element.try_as<Controls::Panel>() || 
                element.try_as<Controls::Button>() || 
                element.try_as<Controls::Image>() || 
                element.try_as<Controls::ScrollViewer>() || 
                element.try_as<Controls::TextBlock>() ||
                element.try_as<Controls::Border>() || 
                element.try_as<Shapes::Shape>() ||
                element.try_as<Controls::ContentPresenter>()))
        {
            return {};
        }

        return TypeHelper::GetDependencyProperties(element);
    }
}
