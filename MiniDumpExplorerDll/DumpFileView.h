#pragma once

#include "DumpFileView.g.h"
#include "SymbolEngineUi.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpFileView : DumpFileViewT<DumpFileView>
    {
        DumpFileView(hstring path, hstring display_name);

        hstring DisplayName() const;
        void DisplayName(hstring value);
        hstring Path() const;
        void Path(hstring value);

        // ReSharper disable once CppRedundantQualifier
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> Categories() const;

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token);

        void OnItemInvoked(muxc::NavigationView const& sender, muxc::NavigationViewItemInvokedEventArgs const& args);
        void ContentFrame_NavigationFailed(Windows::Foundation::IInspectable const& sender,
                                           Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const& args);

    private:
        void SetupInvalidUserModeDump();
        void SetupUserModeDump();
        void SetupX86KernelMemoryDump();
        void SetupX64KernelMemoryDump();

    private:
        hstring display_name_;
        hstring path_;
        /*
        dlg_help_utils::mini_dump dump_file_;
        SymbolEngineUi symbol_engine_ui_;
        dlg_help_utils::dbg_help::symbol_engine symbol_engine{ symbol_engine_ui_ };
        */

        // ReSharper disable once CppRedundantQualifier
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> categories_;
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> property_changed_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DumpFileView : DumpFileViewT<DumpFileView, implementation::DumpFileView>
    {
    };
}
