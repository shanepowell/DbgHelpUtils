#pragma once

#include "StyleExtensions.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct StyleExtensions : StyleExtensionsT<StyleExtensions>
    {
        StyleExtensions();
        static Microsoft::UI::Xaml::ResourceDictionary GetResources(Microsoft::UI::Xaml::Style const& obj);
        static void SetResources(Microsoft::UI::Xaml::Style const& obj, Microsoft::UI::Xaml::ResourceDictionary const& value);
        static Microsoft::UI::Xaml::DependencyProperty ResourcesProperty();

    private:
        static void ResourcesChanged(Microsoft::UI::Xaml::DependencyObject const& sender, Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        static void ForceControlToReloadThemeResources(Microsoft::UI::Xaml::FrameworkElement const& frameworkElement);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct StyleExtensions : StyleExtensionsT<StyleExtensions, implementation::StyleExtensions>
    {
    };
}
