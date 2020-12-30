#pragma once

#include "DumpFileView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpFileView : DumpFileViewT<DumpFileView>
    {
        DumpFileView(hstring path, hstring display_name, IDumpFile dump_file);

        hstring DisplayName() const;
        void DisplayName(hstring value);
        hstring Path() const;
        void Path(hstring value);

        // ReSharper disable once CppRedundantQualifier
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> Categories() const;

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token);

        void OnItemInvoked(muxc::NavigationView const& sender, muxc::NavigationViewItemInvokedEventArgs const& args);

    private:
        void SetupInvalidUserModeDump();
        void SetupUserModeDump();
        void SetupX86KernelMemoryDump();
        void SetupX64KernelMemoryDump();
        void SetupDumpHeaderPage();
        Windows::UI::Xaml::Controls::UserControl CreateStreamPage(IDumpFileStream const& stream) const;

    private:
        hstring display_name_;
        hstring path_;
        IDumpFile dump_file_;

        // ReSharper disable once CppRedundantQualifier
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> categories_{winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>()};
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> property_changed_;
        std::unordered_map<std::wstring, Windows::UI::Xaml::Controls::UserControl> pages_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpFileView : DumpFileViewT<DumpFileView, implementation::DumpFileView>
    {
    };
}
