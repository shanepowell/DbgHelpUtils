#include "pch.h"
#include "MiniDumpPage.xaml.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/wide_runtime_error.h"

#if __has_include("MiniDumpPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace dlg_help_utils::exceptions;
using namespace dlg_help_utils;
using namespace std::string_literals;

namespace
{
    const std::wstring settingsTag = L"Settings"s;
}

namespace winrt::MiniDumpExplorer::implementation
{
    std::unordered_map<std::wstring, Windows::UI::Xaml::Interop::TypeName> MiniDumpPage::pageMap_ =
        {
            { L"Header", xaml_typename<HeaderPage>() },
            { L"Streams", xaml_typename<StreamsPage>() },
            { settingsTag, xaml_typename<SettingsPage>() },
        };


    MiniDumpPage::MiniDumpPage(Windows::Storage::StorageFile const& file)
    {
        InitializeComponent();

        NavigationView().SelectedItem(NavigationView().MenuItems().GetAt(0));

        file_ = file;
        LoadMiniDump(file);
    }

    MiniDumpPage::~MiniDumpPage() = default;

    void MiniDumpPage::NavigationView_SelectionChanged([[maybe_unused]] Controls::NavigationView const& sender, Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        if(const auto navItemTag = args.IsSettingsSelected() ? settingsTag : static_cast<std::wstring>(unbox_value<hstring>(args.SelectedItemContainer().Tag())); 
            !navItemTag.empty())
        {
            if(auto const it = pageMap_.find(navItemTag); it != pageMap_.end())
            {
                ContentFrame().Navigate(it->second);
            }
        }
    }

    fire_and_forget MiniDumpPage::LoadMiniDump(Windows::Storage::StorageFile const& file)
    {
        auto lifetime = get_strong();
        apartment_context ui_thread;

        try
        {
            co_await resume_background();

            miniDump_ = std::make_unique<mini_dump>(static_cast<std::wstring>(file.Path()));

            valid_ = false;
            miniDump_->open_mini_dump();

            valid_ = miniDump_->header();

            if (valid_ && miniDump_->type() == dump_file_type::user_mode_dump)
            {
                co_await ui_thread;

                const Controls::NavigationViewItem item;
                item.Content(box_value(L"Streams"));
                item.Tag(box_value(L"Streams"));
                const Controls::SymbolIcon iconSource;
                iconSource.Symbol(Controls::Symbol::Library);
                item.Icon(iconSource);
                NavigationView().MenuItems().InsertAt(1, item);
            }
        }
        catch (wide_runtime_error const& e)
        {
            valid_ = false;
            openError_ = e.message();
        }
        catch (std::exception const& e)
        {
            valid_ = false;
            openError_ = string_conversation::utf8_to_wstring(e.what());
        }

        if(!openError_.empty())
        {
            co_await ui_thread;

            const Controls::ContentDialog dialog;
            dialog.Title(box_value(L"Error"));
            dialog.Content(box_value(openError_));
            dialog.CloseButtonText(L"Close");
            co_await dialog.ShowAsync();
        }
    }
}
