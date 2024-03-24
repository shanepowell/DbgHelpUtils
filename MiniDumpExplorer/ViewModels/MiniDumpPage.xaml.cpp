#include "pch.h"
#include "MiniDumpPage.xaml.h"

#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Helpers/GlobalOptions.h"

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
    const std::wstring SettingsTag = L"Settings"s;
}

namespace winrt::MiniDumpExplorer::implementation
{
    std::unordered_map<std::wstring, Windows::UI::Xaml::Interop::TypeName> MiniDumpPage::pageMap_ =
        {
            { L"Header", xaml_typename<HeaderPage>() },
            { L"Streams", xaml_typename<StreamsPage>() },
            { SettingsTag, xaml_typename<SettingsPage>() },
        };


    MiniDumpPage::MiniDumpPage(Windows::Storage::StorageFile const& file)
    {
        InitializeComponent();

        NavigationView().SelectedItem(NavigationView().MenuItems().GetAt(0));

        file_ = file;
    }

    MiniDumpPage::~MiniDumpPage() = default;

    void MiniDumpPage::NavigationView_SelectionChanged([[maybe_unused]] Controls::NavigationView const& sender, Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        if(const auto navItemTag = args.IsSettingsSelected() ? SettingsTag : static_cast<std::wstring>(unbox_value<hstring>(args.SelectedItemContainer().Tag())); 
            !navItemTag.empty())
        {
            if(auto const it = pageMap_.find(navItemTag); it != pageMap_.end())
            {
                ContentFrame().Navigate(it->second, *this);
            }
        }
    }

    Windows::Foundation::IAsyncOperation<bool> MiniDumpPage::LoadMiniDump()
    {
        try
        {
            auto const fullPath = static_cast<std::wstring>(file_.Path());
            miniDump_ = std::make_unique<mini_dump>(fullPath);

            valid_ = false;
            miniDump_->open_mini_dump();

            GlobalOptions::Options().AddRecentFile(fullPath);

            valid_ = miniDump_->header();

            if (valid_ && miniDump_->type() == dump_file_type::user_mode_dump && miniDump_->directory() != nullptr)
            {
                Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
                auto const streamsTitleName = rm.MainResourceMap().GetValue(L"Resources/StreamsTitleName").ValueAsString();
                const Controls::NavigationViewItem item;
                item.Content(box_value(streamsTitleName));
                item.Tag(box_value(streamsTitleName));
                const Controls::SymbolIcon iconSource;
                iconSource.Symbol(Controls::Symbol::Library);
                item.Icon(iconSource);
                NavigationView().MenuItems().InsertAt(1, item);
            }

            miniDumpLoadedHandler_(*this, MiniDumpExplorer::MiniDumpPage{*this});
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
            const Controls::ContentDialog dialog;
            Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            dialog.Title(box_value(rm.MainResourceMap().GetValue(L"Resources/LoadMiniDumpErrorTitle").ValueAsString()));
            dialog.Content(box_value(openError_));
            dialog.CloseButtonText(rm.MainResourceMap().GetValue(L"Resources/LoadMiniDumpErrorCloseButton").ValueAsString());
            co_await dialog.ShowAsync();

            co_return false;
        }

        co_return true;
    }

    event_token MiniDumpPage::MiniDumpLoaded(Windows::Foundation::EventHandler<MiniDumpExplorer::MiniDumpPage> const& value)
    {
        return miniDumpLoadedHandler_.add(value);
    }

    void MiniDumpPage::MiniDumpLoaded(event_token const& value)
    {
        miniDumpLoadedHandler_.remove(value);
    }
}
