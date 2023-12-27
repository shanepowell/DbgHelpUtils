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
        dialog.XamlRoot(Content().XamlRoot());
        dialog.Title(box_value(L"Quit MiniDump Explorer"));
        dialog.Content(box_value(L"Are you sure?"));
        dialog.PrimaryButtonText(L"Quit");
        dialog.SecondaryButtonText(L"Cancel");
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

    event_token MainWindow::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChanged_.add(handler);
    }

    void MainWindow::PropertyChanged(event_token const& token) noexcept
    {
        propertyChanged_.remove(token);
    }

    void MainWindow::OpenFileInTab(Windows::Storage::StorageFile const& file)
    {
        TabView().TabItems().Append(CreateNewTab(file));
        TabView().SelectedItem(TabView().TabItems().GetAt(TabView().TabItems().Size() - 1));
    }

    void MainWindow::OpenDefaultTab()
    {
        Controls::TabViewItem const newItem;
        Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        auto const headerString = rm.MainResourceMap().GetValue(L"Resources/DefaultTabHeader").ValueAsString();
        newItem.Header(box_value(headerString));
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

        OpenFileInTab(file);
    }

    void MainWindow::RaisePropertyChanged(hstring const& propertyName)
    {
        propertyChanged_(*this, Data::PropertyChangedEventArgs(propertyName));
    }

    Controls::TabViewItem MainWindow::CreateNewTab(Windows::Storage::StorageFile const& file)
    {
        Controls::TabViewItem newItem;
        newItem.Header(box_value(file.Name()));
        const Controls::SymbolIconSource iconSource;
        iconSource.Symbol(Controls::Symbol::Document);
        newItem.IconSource(iconSource);
        newItem.Content(winrt::make<MiniDumpPage>(file));
        return newItem;
    }
}
