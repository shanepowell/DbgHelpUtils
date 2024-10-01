#pragma once

#include "DefaultPage.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DefaultPage : DefaultPageT<DefaultPage>
    {
        DefaultPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();
        void NavigationView_SelectionChanged(Microsoft::UI::Xaml::Controls::NavigationView const& sender, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DefaultPage : DefaultPageT<DefaultPage, implementation::DefaultPage>
    {
    };
}
