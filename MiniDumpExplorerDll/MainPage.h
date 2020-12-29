#pragma once

#include "MainPage.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage(IDumpFileFactory factory);

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token);

        Windows::Foundation::IAsyncAction TabView_AddTabButtonClick(muxc::TabView const& sender, Windows::Foundation::IInspectable const& args) const;
        static void TabView_TabCloseRequested(muxc::TabView const& sender, muxc::TabViewTabCloseRequestedEventArgs const& args);

        bool DeferredSymbolLoadCancel(hstring module_name);
        void DeferredSymbolLoadPartial(hstring module_name);
        void StartDownload(hstring module_name);
        void DownloadPercent(uint32_t percent);
        void DownloadComplete();

    private:
        IDumpFileFactory factory_;
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
