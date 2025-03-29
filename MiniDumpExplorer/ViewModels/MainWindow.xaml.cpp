// ReSharper disable CppClangTidyHicppExceptionBaseclass

#include "pch.h"
#include "MainWindow.xaml.h"

#include <shobjidl.h>
#include <Microsoft.Ui.Xaml.Window.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.System.h>

#include "DefaultPage.xaml.h"
#include "MiniDumpPage.xaml.h"
#include "Utility/logger.h"
#include "Helpers/WindowHelper.h"

#include <chrono>

#if __has_include("MainWindow.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MainWindow.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::chrono_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        Windows::Graphics::RectInt32 GetRect(Windows::Foundation::Rect const& bounds, double const scale)
        {
            return {
                .X = static_cast<int>(std::lround(bounds.X * scale)),
                .Y = static_cast<int>(std::lround(bounds.Y * scale)),
                .Width = static_cast<int>(std::lround(bounds.Width * scale)),
                .Height = static_cast<int>(std::lround(bounds.Height * scale))
            };
        }
    }


    MainWindow::MainWindow()
    {
        logger::Log().LogMessage(log_level::debug, "MainWindow");
        ExtendsContentIntoTitleBar(true);
        SetTitleBar(AppTitleBar());
        auto appTitleName = Application::Current().Resources().Lookup(box_value(L"AppTitleName")).as<hstring>();
        Title(appTitleName);
    }

    void MainWindow::InitializeComponent()
    {
        MainWindowT::InitializeComponent();
        OpenDefaultTab();
        logger::Log().LogMessage(log_level::debug, "MainWindow InitializeComponent complete");

        activatedEvent_ = Activated({ this, &MainWindow::MainWindow_Activated });
        appWindowChangedEvent_ = AppWindow().Changed({ this, &MainWindow::AppWindow_Changed });
        loadedEvent_ = AppTitleBar().Loaded({ this, &MainWindow::AppTitleBar_OnLoaded });
        sizeChangedEvent_ = AppTitleBar().SizeChanged({ this, &MainWindow::AppTitleBar_OnSizeChanged });

        AppWindow().TitleBar().PreferredHeightOption(Microsoft::UI::Windowing::TitleBarHeightOption::Tall);
    }

    void MainWindow::MainWindow_Activated([[maybe_unused]] Windows::Foundation::IInspectable const& sender, WindowActivatedEventArgs const& args)
    {
        if (args.WindowActivationState() == WindowActivationState::Deactivated)
        {
            auto brush = Application::Current().Resources().Lookup(box_value(L"WindowCaptionForegroundDisabled")).as<Media::SolidColorBrush>();
            TitleBarTextBlock().Foreground(brush);
            TabTitleBarTextBlock().Foreground(brush);
        }
        else
        {
            auto brush = Application::Current().Resources().Lookup(box_value(L"WindowCaptionForeground")).as<Media::SolidColorBrush>();
            TitleBarTextBlock().Foreground(brush);
            TabTitleBarTextBlock().Foreground(brush);
        }
    }

    void MainWindow::AppWindow_Changed(Microsoft::UI::Windowing::AppWindow const& sender, Microsoft::UI::Windowing::AppWindowChangedEventArgs const& args)
    {
        if (args.DidPresenterChange())
        {
            using Microsoft::UI::Windowing::AppWindowPresenterKind;
            switch (sender.Presenter().Kind())
            {
            case AppWindowPresenterKind::CompactOverlay:
                // Compact overlay - hide custom title bar
                // and use the default system title bar instead.
                AppTitleBar().Visibility(Visibility::Collapsed);
                sender.TitleBar().ResetToDefault();
                break;

            case AppWindowPresenterKind::FullScreen:
                // Full screen - hide the custom title bar
                // and the default system title bar.
                AppTitleBar().Visibility(Visibility::Collapsed);
                sender.TitleBar().ExtendsContentIntoTitleBar(true);
                break;

            case AppWindowPresenterKind::Overlapped:
                // Normal - hide the system title bar
                // and use the custom title bar instead.
                AppTitleBar().Visibility(Visibility::Visible);
                sender.TitleBar().ExtendsContentIntoTitleBar(true);
                break;

            default:
                // Use the default system title bar.
                sender.TitleBar().ResetToDefault();
                break;
            }
        }
    }

    void MainWindow::AppTitleBar_OnLoaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
    {
        if (ExtendsContentIntoTitleBar())
        {
            SetRegionsForCustomTitleBar();
        }
    }

    void MainWindow::AppTitleBar_OnSizeChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] SizeChangedEventArgs const& args)
    {
        if (ExtendsContentIntoTitleBar())
        {
            SetRegionsForCustomTitleBar();
        }
    }


    fire_and_forget MainWindow::TitleBarIcon_PointerPressed([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& args)
    {
        if (auto const ptr = args.Pointer();
            ptr.PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Mouse)
        {
            auto const ptrPt = args.GetCurrentPoint(TitleBarIcon());
            if (auto const properties = ptrPt.Properties();
                properties.IsLeftButtonPressed())
            {
                auto weak_self = get_weak();
                apartment_context ui_thread;
                co_await resume_after(500ms);
                co_await ui_thread;

                if(auto const anchor_self = weak_self.get();
                    anchor_self && !WindowHelper::IsExiting())
                {
                    ShowSystemMenu();
                }
            }
            else if (properties.IsRightButtonPressed())
            {
                ShowSystemMenu();
            }
        }
    }

    void MainWindow::TitleBarIcon_MouseDoubleClick([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args) const
    {
        Close();
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
        newItem.IsClosable(false);
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
        const auto& file = co_await picker.PickSingleFileAsync();
        if(!file)
        {
            co_return;
        }

        co_await OpenFileInTab(file);
    }

    void MainWindow::SetRegionsForCustomTitleBar()
    {
        // Specify the interactive regions of the title bar.

        auto const scaleAdjustment = AppTitleBar().XamlRoot().RasterizationScale();

        RightPaddingColumn().Width(GridLength(AppWindow().TitleBar().RightInset() / scaleAdjustment));
        LeftPaddingColumn().Width(GridLength(AppWindow().TitleBar().LeftInset() / scaleAdjustment));

        // Get the rectangle around the TitleBarTextArea grid.
        auto transform = TitleBarTextArea().TransformToVisual(nullptr);
        auto bounds = transform.TransformBounds(Windows::Foundation::Rect(
                0,
                0,
                static_cast<float>(TitleBarTextArea().ActualWidth()),
                static_cast<float>(TitleBarTextArea().ActualHeight())
            ));
        auto titleBarTextAreaRect = GetRect(bounds, scaleAdjustment);

        AppWindow().TitleBar().SetDragRectangles({titleBarTextAreaRect});

        // Get the rectangle around the TitleBarIcon control.
        transform = TitleMenuBar().TransformToVisual(nullptr);
        bounds = transform.TransformBounds(Windows::Foundation::Rect(
                0,
                0,
                static_cast<float>(TitleBarIcon().ActualWidth()),
                static_cast<float>(TitleBarIcon().ActualHeight())
            ));
        auto titleBarIconRect = GetRect(bounds, scaleAdjustment);

        // Get the rectangle around the MenuBar control.
        transform = TitleMenuBar().TransformToVisual(nullptr);
        bounds = transform.TransformBounds(Windows::Foundation::Rect(
                0,
                0,
                static_cast<float>(TitleMenuBar().ActualWidth()),
                static_cast<float>(TitleMenuBar().ActualHeight())
            ));
        auto titleMenuBarRect = GetRect(bounds, scaleAdjustment);

        auto nonClientInputSrc = Microsoft::UI::Input::InputNonClientPointerSource::GetForWindowId(AppWindow().Id());
        nonClientInputSrc.SetRegionRects(Microsoft::UI::Input::NonClientRegionKind::Passthrough, { titleBarIconRect, titleMenuBarRect });
    }

    void MainWindow::ShowSystemMenu() const
    {
        const auto windowNative{ try_as<IWindowNative>() };
        HWND hWnd{ nullptr };
        windowNative->get_WindowHandle(&hWnd);

        if (hWnd)
        {
            if (HMENU const hMenu = GetSystemMenu(hWnd, FALSE);
                hMenu)
            {
                if (IsZoomed(hWnd))
                {
                    EnableMenuItem(hMenu, SC_MAXIMIZE, MF_GRAYED);
                    EnableMenuItem(hMenu, SC_RESTORE, MF_ENABLED);
                }
                else
                {
                    EnableMenuItem(hMenu, SC_MAXIMIZE, MF_ENABLED);
                    EnableMenuItem(hMenu, SC_RESTORE, MF_GRAYED);
                }
                POINT pt;
                GetCursorPos(&pt);
                int command = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, nullptr);  // NOLINT(misc-redundant-expression)
                if (command == 0)
                {
                    return;
                }

                PostMessage(hWnd, WM_SYSCOMMAND, command, NULL);
            }
        }
    }

    Windows::Foundation::IAsyncOperation<Controls::TabViewItem> MainWindow::CreateNewTab(Windows::Storage::StorageFile const file)  // NOLINT(performance-unnecessary-value-param)
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
