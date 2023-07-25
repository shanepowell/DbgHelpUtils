#pragma once

#include "DefaultPage.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DefaultPage : DefaultPageT<DefaultPage>
    {
        DefaultPage();

        void NavigationView_SelectionChanged(Microsoft::UI::Xaml::Controls::NavigationView const& sender, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);

    private:
        static std::unordered_map<std::wstring, Windows::UI::Xaml::Interop::TypeName> pageMap_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DefaultPage : DefaultPageT<DefaultPage, implementation::DefaultPage>
    {
    };
}
