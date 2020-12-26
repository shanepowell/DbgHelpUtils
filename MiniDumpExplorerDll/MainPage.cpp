#include "pch.h"
#include "MainPage.h"
#if __has_include("MainPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MainPage.g.cpp"
#endif

#include "DumpFileView.h"
#include "WindowHelper.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::Storage::Pickers;

namespace winrt::MiniDumpExplorer::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent(); // NOLINT(clang-diagnostic-undefined-func-template)
    }

    event_token MainPage::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return property_changed_.add(handler);
    }

    void MainPage::PropertyChanged(event_token const& token)
    {
        property_changed_.remove(token);
    }

    IAsyncAction MainPage::TabView_AddTabButtonClick([[maybe_unused]] muxc::TabView const& sender,
                                                     [[maybe_unused]] Windows::Foundation::IInspectable const& args) const {
        FileOpenPicker const picker;
        WindowHelper::SetupFileOpenPicker(picker);
        picker.ViewMode(PickerViewMode::Thumbnail);
        picker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);
        picker.FileTypeFilter().Append(L".dmp");

        // save tab view com reference before the co_wait which will invalid the sender object
        auto const tab_view = sender; // NOLINT(performance-unnecessary-copy-initialization)

        auto const file = co_await picker.PickSingleFileAsync();
        if (file)
        {
            muxc::TabViewItem const new_tab;
            muxc::SymbolIconSource icon_source;
            icon_source.Symbol(Controls::Symbol::Document);
            new_tab.IconSource(std::move(icon_source));
            new_tab.Header(box_value(file.DisplayName()));
            auto const dump_file_view = winrt::make_self<DumpFileView>(file.DisplayName(), file.Path());
            new_tab.Content(*dump_file_view);

            auto const tab_index = tab_view.TabItems().Size();
            tab_view.TabItems().InsertAt(tab_index, new_tab);
            tab_view.SelectedIndex(static_cast<int32_t>(tab_index));
        }
    }

    void MainPage::TabView_TabCloseRequested([[maybe_unused]] muxc::TabView const& sender,
                                             [[maybe_unused]] muxc::TabViewTabCloseRequestedEventArgs const& args)
    {
        uint32_t index;
        sender.TabItems().IndexOf(args.Tab(), index);
        sender.TabItems().RemoveAt(index);
    }
}
