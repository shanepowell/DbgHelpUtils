#include "pch.h"
#include "BindingInfo.h"


using namespace winrt::Microsoft::UI::Xaml;

namespace Utility
{

    BindingInfo::BindingInfo() = default;

    BindingInfo::BindingInfo(Data::BindingExpression const& bindingExpression, DependencyProperty const& bindingTarget, FrameworkElement const& element)
        : bindingExpression_{bindingExpression}
        , bindingTarget_{bindingTarget}
        , element_{element}
    {
    }

    Data::BindingExpression BindingInfo::BindingExpression() const
    {
        return bindingExpression_;
    }

    void BindingInfo::BindingExpression(Data::BindingExpression const& value)
    {
        bindingExpression_ = value;
    }

    DependencyProperty BindingInfo::BindingTarget() const
    {
        return bindingTarget_;
    }

    void BindingInfo::BindingTarget(DependencyProperty const& value)
    {
        bindingTarget_ = value;
    }

    FrameworkElement BindingInfo::Element() const
    {
        return element_;
    }

    void BindingInfo::Element(FrameworkElement const& value)
    {
        element_ = value;
    }
}
