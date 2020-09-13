#pragma once

#include "MainPage.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token);

        Windows::Foundation::IAsyncAction TabView_AddTabButtonClick(muxc::TabView const& sender,
                                                                    Windows::Foundation::IInspectable const& args) const;
        void TabView_TabCloseRequested(muxc::TabView const& sender,
                                       muxc::TabViewTabCloseRequestedEventArgs const& args);

    private:
        hstring display_name_;
        hstring path_;
        // ReSharper disable once CppRedundantQualifier
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> categories_;
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> property_changed_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
