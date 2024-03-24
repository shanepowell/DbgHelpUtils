// ReSharper disable CppClangTidyHicppExceptionBaseclass

#include "pch.h"
#include "MainWindow.xaml.h"

#include <shobjidl.h>
#include <Microsoft.Ui.Xaml.Window.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.Storage.Pickers.h>

#include "DefaultPage.xaml.h"
#include "MiniDumpPage.xaml.h"
#include "Utility/logger.h"

#if __has_include("MainWindow.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MainWindow.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MainWindow::MainWindow()
    {
        logger::Log().LogMessage(log_level::debug, "MainWindow");
        InitializeComponent();
        ExtendsContentIntoTitleBar(true);
        SetTitleBar(AppTitleBar());
        OpenDefaultTab();
        logger::Log().LogMessage(log_level::debug, "MainWindow post setup");
    }

    fire_and_forget MainWindow::MenuFileOpen_Click(Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
        co_await OpenFileInNewTab();
    }

    fire_and_forget MainWindow::MenuFileOpen_Exit(Windows::Foundation::IInspectable const&, RoutedEventArgs const&) const
    {
        const Controls::ContentDialog dialog;
        Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        dialog.XamlRoot(Content().XamlRoot());
        dialog.Title(box_value(rm.MainResourceMap().GetValue(L"Resources/QuitMiniDumpExplorerDialogTitle").ValueAsString()));
        dialog.Content(box_value(rm.MainResourceMap().GetValue(L"Resources/QuitMiniDumpExplorerDialogContent").ValueAsString()));
        dialog.PrimaryButtonText(rm.MainResourceMap().GetValue(L"Resources/QuitMiniDumpExplorerDialogPrimaryButtonText").ValueAsString());
        dialog.SecondaryButtonText(rm.MainResourceMap().GetValue(L"Resources/QuitMiniDumpExplorerDialogSecondaryButtonText").ValueAsString());
        dialog.DefaultButton(Controls::ContentDialogButton::Primary);
        if(co_await dialog.ShowAsync() == Controls::ContentDialogResult::Primary)
        {
            Close();
        }
    }

    fire_and_forget MainWindow::TabView_AddTabButtonClick(Controls::TabView const&, Windows::Foundation::IInspectable const&)
    {
        co_await OpenFileInNewTab();
    }

    void MainWindow::TabView_SelectionChanged(Windows::Foundation::IInspectable const&, Controls::SelectionChangedEventArgs const&)
    {
        RaisePropertyChanged( L"SelectedTabTitle" );
    }

    void MainWindow::TabView_TabCloseRequested(Controls::TabView const& sender, Controls::TabViewTabCloseRequestedEventArgs const& args)
    {
        if (uint32_t indexOf; sender.TabItems().IndexOf(args.Item(), indexOf))
        {
            sender.TabItems().RemoveAt(indexOf);
            if(TabView().SelectedIndex() == static_cast<int32_t>(indexOf))
            {
                TabView().SelectedItem(TabView().TabItems().GetAt(indexOf - 1));
            }
        }
    }

    hstring MainWindow::SelectedTabTitle()
    {
        if (const auto selectedItem{ TabView().SelectedItem() })
        {
            if (const auto header{ selectedItem.as<Controls::TabViewItem>().Header() })
            {
                return L" - " + header.as<hstring>();
            }
        }
        return L"";
    }

    Windows::Foundation::IAsyncAction MainWindow::OpenFileInTab(Windows::Storage::StorageFile const& file)
    {
        if(auto const tabItem = co_await CreateNewTab(file);
            tabItem)
        {
            TabView().TabItems().Append(tabItem);
            TabView().SelectedItem(TabView().TabItems().GetAt(TabView().TabItems().Size() - 1));
        }
    }

    void MainWindow::OpenDefaultTab()
    {
        Controls::TabViewItem const newItem;
        Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        newItem.Header(box_value(rm.MainResourceMap().GetValue(L"Resources/DefaultTabHeader").ValueAsString()));
        const Controls::SymbolIconSource iconSource;
        iconSource.Symbol(Controls::Symbol::Admin);
        newItem.IconSource(iconSource);
        newItem.Content(winrt::make<DefaultPage>());

        TabView().TabItems().Append(newItem);
        TabView().SelectedItem(TabView().TabItems().GetAt(0));
    }

    Windows::Foundation::IAsyncAction MainWindow::OpenFileInNewTab()
    {
        const auto windowNative{ this->try_as<IWindowNative>() };
        HWND hWnd{ nullptr };
        windowNative->get_WindowHandle(&hWnd);

        const Windows::Storage::Pickers::FileOpenPicker picker;

        const auto initializeWithWindow{ picker.as<IInitializeWithWindow>() };
        initializeWithWindow->Initialize(hWnd);

        picker.FileTypeFilter().Append(L".dmp");
        picker.FileTypeFilter().Append(L".mdmp");
        picker.ViewMode(Windows::Storage::Pickers::PickerViewMode::List);
        picker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Downloads);
        const auto file = co_await picker.PickSingleFileAsync();
        if(!file)
        {
            co_return;
        }

        co_await OpenFileInTab(file);
    }

    Windows::Foundation::IAsyncOperation<Controls::TabViewItem> MainWindow::CreateNewTab(Windows::Storage::StorageFile const& file)
    {
        auto miniDumpPage = winrt::make<MiniDumpPage>(file);
        if(!co_await miniDumpPage.LoadMiniDump())
        {
            co_return nullptr;
        }

        Controls::TabViewItem newItem;
        newItem.Header(box_value(file.Name()));
        const Controls::SymbolIconSource iconSource;
        iconSource.Symbol(Controls::Symbol::Document);
        newItem.IconSource(iconSource);
        newItem.Content(miniDumpPage);
        co_return newItem;
    }
}
