#pragma once

#include <generator>

namespace TypeHelper
{
    static constexpr wchar_t LeftIndexerToken = '[';
    static constexpr wchar_t PropertyNameSeparator = '.';
    static constexpr wchar_t RightIndexerToken = ']';

    std::wstring GetBindingPropertyName(winrt::Microsoft::UI::Xaml::Data::Binding const& binding);

    std::generator<winrt::Microsoft::UI::Xaml::DependencyProperty> GetDependencyProperties(winrt::guid const& id);

    template<typename T>
    std::generator<winrt::Microsoft::UI::Xaml::DependencyProperty> GetDependencyProperties()
    {
        return GetDependencyProperties(winrt::guid_of<T>());
    }


    std::generator<winrt::Microsoft::UI::Xaml::DependencyProperty> GetDependencyProperties(winrt::Windows::Foundation::IInspectable const& object);

    winrt::Microsoft::UI::Xaml::DependencyProperty GetDependencyProperty(winrt::Windows::Foundation::IInspectable const& object, std::wstring_view const& name);
    winrt::Microsoft::UI::Xaml::DependencyProperty GetDependencyProperty(winrt::guid const& id, std::wstring_view const& name);

    template<typename T>
    winrt::Microsoft::UI::Xaml::DependencyProperty GetDependencyProperty(std::wstring_view const& name)
    {
        return GetDependencyProperty(winrt::guid_of<T>(), name);
    }

    winrt::Windows::Foundation::IInspectable GetDependencyPropertyValue(winrt::Windows::Foundation::IInspectable const& object, winrt::Microsoft::UI::Xaml::DependencyProperty const& dependencyProperty);
    winrt::Windows::Foundation::IInspectable GetNestedDependencyPropertyValue(winrt::Windows::Foundation::IInspectable object, std::wstring_view const& propertyPath);
    void SetDependencyPropertyValue(winrt::Windows::Foundation::IInspectable const& object, winrt::Microsoft::UI::Xaml::DependencyProperty const& dependencyProperty, winrt::Windows::Foundation::IInspectable const& value);
    void SetNestedDependencyPropertyValue(winrt::Windows::Foundation::IInspectable& item, winrt::Windows::Foundation::IInspectable const& newValue, std::wstring_view const& propertyPath);

    void LoadAddApplicationDependencyProperties();

    std::wstring PrependDefaultMemberName(winrt::Windows::Foundation::IInspectable const& item, std::wstring const& property);
    std::wstring RemoveDefaultMemberName(std::wstring const& property);
}
