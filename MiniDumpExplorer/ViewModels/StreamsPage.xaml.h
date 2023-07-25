#pragma once

#include "StreamsPage.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct StreamsPage : StreamsPageT<StreamsPage>
    {
        StreamsPage();

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct StreamsPage : StreamsPageT<StreamsPage, implementation::StreamsPage>
    {
    };
}
