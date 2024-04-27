#include "pch.h"
#include "MiniDumpPage.xaml.h"

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/mini_dump_stream_type.h"
#include "DbgHelpUtils/string_compare.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Helpers/GlobalOptions.h"
#include "Models/FileCrc32.h"
#include "Utility/logger.h"

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
    auto const HeaderTag = L"Header"s;
    auto const SettingsTag = L"Settings"s;
    auto const StreamsTag = L"Streams"s;
    auto const UnsupportedStreamTag = L"UnsupportedStream"s;

    std::unordered_map<MINIDUMP_STREAM_TYPE, Controls::Symbol> StreamTypeToIcon =
        {
            {ThreadListStream, Controls::Symbol::ThreeBars},
            {ModuleListStream, Controls::Symbol::Bullets},
            {MemoryListStream, Controls::Symbol::Bookmarks},
            {ExceptionStream, Controls::Symbol::Repair},
            {SystemInfoStream, Controls::Symbol::ContactInfo},
            {ThreadExListStream, Controls::Symbol::ThreeBars},
            {Memory64ListStream, Controls::Symbol::Bookmarks},
            {CommentStreamA, Controls::Symbol::Comment},
            {CommentStreamW, Controls::Symbol::Comment},
            {HandleDataStream, Controls::Symbol::Directions},
            {FunctionTableStream, Controls::Symbol::FourBars},
            {UnloadedModuleListStream, Controls::Symbol::Undo},
            {MiscInfoStream, Controls::Symbol::MusicInfo},
            {MemoryInfoListStream, Controls::Symbol::Admin},
            {ThreadInfoListStream, Controls::Symbol::Admin},
            {HandleOperationListStream, Controls::Symbol::Calendar},
            {TokenStream, Controls::Symbol::Character},
            {SystemMemoryInfoStream, Controls::Symbol::Favorite},
            {ProcessVmCountersStream, Controls::Symbol::DockBottom},
            {ThreadNamesStream, Controls::Symbol::List},
        };

}

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPageParameters;

    std::unordered_map<std::wstring, Windows::UI::Xaml::Interop::TypeName> MiniDumpPage::pageMap_ =
        {
            { HeaderTag, xaml_typename<HeaderPage>() },
            { StreamsTag, xaml_typename<StreamsPage>() },
            { SettingsTag, xaml_typename<SettingsPage>() },
            { mini_dump_stream_type::enum_names::ThreadListStream, xaml_typename<ThreadListStreamPage>() },
            { mini_dump_stream_type::enum_names::ModuleListStream, xaml_typename<ModuleListStreamPage>() },
            { mini_dump_stream_type::enum_names::MemoryListStream, xaml_typename<MemoryListStreamPage>() },
            { mini_dump_stream_type::enum_names::ExceptionStream, xaml_typename<ExceptionStreamPage>() },
            { mini_dump_stream_type::enum_names::SystemInfoStream, xaml_typename<SystemInfoStreamPage>() },
            { mini_dump_stream_type::enum_names::ThreadExListStream, xaml_typename<ThreadExListStreamPage>() },
            { mini_dump_stream_type::enum_names::Memory64ListStream, xaml_typename<Memory64ListStreamPage>() },
            { mini_dump_stream_type::enum_names::CommentStreamA, xaml_typename<CommentStreamPage>() },
            { mini_dump_stream_type::enum_names::CommentStreamW, xaml_typename<CommentStreamPage>() },
            //{ mini_dump_stream_type::enum_names::HandleDataStream, xaml_typename<HandleDataStreamPage>() },
            //{ mini_dump_stream_type::enum_names::FunctionTableStream, xaml_typename<FunctionTableStreamPage>() },
            { mini_dump_stream_type::enum_names::UnloadedModuleListStream, xaml_typename<UnloadedModuleListStreamPage>() },
            { mini_dump_stream_type::enum_names::MiscInfoStream, xaml_typename<MiscInfoStreamPage>() },
            { mini_dump_stream_type::enum_names::MemoryInfoListStream, xaml_typename<MemoryInfoListStreamPage>() },
            { mini_dump_stream_type::enum_names::ThreadInfoListStream, xaml_typename<ThreadInfoListStreamPage>() },
            //{ mini_dump_stream_type::enum_names::HandleOperationListStream, xaml_typename<HandleOperationListStreamPage>() },
            //{ mini_dump_stream_type::enum_names::TokenStream, xaml_typename<TokenStreamPage>() },
            //{ mini_dump_stream_type::enum_names::SystemMemoryInfoStream, xaml_typename<SystemMemoryInfoStreamPage>() },
            //{ mini_dump_stream_type::enum_names::ProcessVmCountersStream, xaml_typename<ProcessVmCountersStreamPage>() },
            //{ mini_dump_stream_type::enum_names::ThreadNamesStream, xaml_typename<ThreadNamesStreamPage>() },
            { UnsupportedStreamTag, xaml_typename<UnsupportedStreamPage>() },
        };


    MiniDumpPage::MiniDumpPage(Windows::Storage::StorageFile const& file)
    {
        file_ = file;
    }

    MiniDumpPage::~MiniDumpPage() = default;

    void MiniDumpPage::InitializeComponent()
    {
        MiniDumpPageT::InitializeComponent();

        NavigationView().SelectedItem(NavigationView().MenuItems().GetAt(0));
    }

    void MiniDumpPage::NavigationView_SelectionChanged([[maybe_unused]] Controls::NavigationView const& sender, Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        auto navigationItemTag = args.IsSettingsSelected() ? SettingsTag : static_cast<std::wstring>(unbox_value_or<hstring>(args.SelectedItemContainer().Tag(), {}));
        MiniDumpExplorer::MiniDumpPageParameters parameters{nullptr};
        if(navigationItemTag.empty())
        {
            parameters = args.SelectedItemContainer().Tag().try_as<MiniDumpExplorer::MiniDumpPageParameters>();
            if(parameters)
            {
                navigationItemTag = parameters.NavigationItemTag();
            }
        }

        if(!navigationItemTag.empty())
        {
            if(!parameters)
            {
                parameters = MiniDumpExplorer::MiniDumpPageParameters{*this, navigationItemTag, 0};
            }

            if(auto const it = pageMap_.find(navigationItemTag); it != pageMap_.end())
            {
                ContentFrame().Navigate(it->second, parameters);
            }
        }
    }

    Windows::Foundation::IAsyncOperation<bool> MiniDumpPage::LoadMiniDump()
    {
        Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        try
        {
            auto self = get_strong();

            apartment_context ui_thread; 
            co_await resume_background();

            logger::Log().LogMessage(log_level::info, std::format(L"Loading MiniDump: {}", file_.Path()));
            auto const fullPath = static_cast<std::wstring>(file_.Path());
            miniDump_ = std::make_shared<mini_dump>(fullPath);

            valid_ = false;
            miniDump_->open_mini_dump();

            valid_ = miniDump_->is_valid();
            auto const* directory = miniDump_->directory();

            if(!valid_)
            {
                openError_ = rm.MainResourceMap().GetValue(L"Resources/InvalidMinidumpFile").ValueAsString();
            }
            logger::Log().LogMessage(log_level::debug, L"MiniDump Opened: {}");

            co_await ui_thread;

            fileCrc32_.as<FileCrc32>()->Set(miniDump_);
            GlobalOptions::Options().AddRecentFile(fullPath);

            if (valid_ && miniDump_->type() == dump_file_type::user_mode_dump && directory != nullptr)
            {
                logger::Log().LogMessage(log_level::debug, L"MiniDump Setup UI Elements");
                auto const streamsTitleName = rm.MainResourceMap().GetValue(L"Resources/StreamsTitleName").ValueAsString();
                const Controls::NavigationViewItem item;
                item.Content(box_value(streamsTitleName));
                item.Tag(box_value(StreamsTag));
                const Controls::SymbolIcon iconSource;
                iconSource.Symbol(Controls::Symbol::Library);
                item.Icon(iconSource);
                NavigationView().MenuItems().InsertAt(1, item);

                auto const* header = miniDump_->header();

                logger::Log().LogMessage(log_level::debug, L"MiniDump Setup Stream Entry UI Elements");
                for (uint32_t index = 0; index < header->NumberOfStreams; ++index)
                {
                    auto const& stream_entry = directory[index];
                    if(auto const streamItem = CreateNavigationViewItemForStreamType(static_cast<MINIDUMP_STREAM_TYPE>(stream_entry.StreamType), index, stream_entry.Location);
                        streamItem)
                    {
                        item.MenuItems().Append(streamItem);
                    }
                }
                logger::Log().LogMessage(log_level::debug, L"MiniDump UI Elements Setup");
            }

            logger::Log().LogMessage(log_level::debug, L"Trigger MiniDump Loaded handler");
            miniDumpLoadedHandler_(*this, MiniDumpExplorer::MiniDumpPage{*this});
            logger::Log().LogMessage(log_level::debug, L"MiniDump Loaded handler complete");
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
            dialog.Title(box_value(rm.MainResourceMap().GetValue(L"Resources/LoadMiniDumpErrorTitle").ValueAsString()));
            dialog.Content(box_value(openError_));
            dialog.CloseButtonText(rm.MainResourceMap().GetValue(L"Resources/LoadMiniDumpErrorCloseButton").ValueAsString());
            co_await dialog.ShowAsync();

            co_return false;
        }

        co_return true;
    }

    void MiniDumpPage::SelectNavigationItemTag(std::wstring const& tag)
    {
        if(auto const action = FindAndSelectNavigationTag(tag, NavigationView().MenuItems());
            action)
        {
            action();
        }
    }

    event_token MiniDumpPage::MiniDumpLoaded(Windows::Foundation::EventHandler<MiniDumpExplorer::MiniDumpPage> const& value)
    {
        return miniDumpLoadedHandler_.add(value);
    }

    void MiniDumpPage::MiniDumpLoaded(event_token const& value)
    {
        miniDumpLoadedHandler_.remove(value);
    }

    std::function<void()> MiniDumpPage::FindAndSelectNavigationTag(std::wstring const& tag, Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> const& menuItems)
    {
        for (auto const& item : menuItems)
        {
            if (auto const menuItem = item.try_as<Controls::NavigationViewItem>();
                menuItem)
            {
                if(auto const parameters = menuItem.Tag().try_as<MiniDumpExplorer::MiniDumpPageParameters>();
                    parameters && parameters.NavigationItemTag() == tag)
                {
                    return [this, menuItem]()
                    {
                        NavigationView().SelectedItem(menuItem);
                    };
                }

                if(auto const itemTag = unbox_value_or<hstring>(menuItem.Tag(), L"");
                    !itemTag.empty() && string_utils::equals(itemTag, tag))
                {
                    return [this, menuItem]()
                    {
                        NavigationView().SelectedItem(menuItem);
                    };
                }

                
                if(auto action = FindAndSelectNavigationTag(tag, menuItem.MenuItems());
                    action)
                {
                    return [action = std::move(action), menuItem]()
                    {
                        menuItem.IsExpanded(true);
                        action();
                    };
                }
            }
        }
        return {};
    }

    Controls::NavigationViewItem MiniDumpPage::CreateNavigationViewItemForStreamType(MINIDUMP_STREAM_TYPE const stream_type, uint32_t const stream_index, MINIDUMP_LOCATION_DESCRIPTOR const& location) const
    {
        if(location.DataSize == 0)
        {
            return {nullptr};
        }

        auto const title = mini_dump_stream_type::to_wstring(stream_type);
        Controls::Symbol icon;
        std::wstring tag;
        if(auto it = StreamTypeToIcon.find(stream_type); it != StreamTypeToIcon.end())
        {
            icon = it->second;
            tag = mini_dump_stream_type::to_enum_wstring(stream_type);
        }
        else
        {
            icon = Controls::Symbol::DisableUpdates;
            tag = UnsupportedStreamTag;
        }

        return CreateNavigationViewItemForStream(title, tag, stream_index, icon);
    }

    Controls::NavigationViewItem MiniDumpPage::CreateNavigationViewItemForStream(std::wstring const& title, std::wstring const& tag, uint32_t const stream_index, Controls::Symbol const symbol) const
    {
        const Controls::NavigationViewItem item;
        item.Content(box_value(hstring{title}));
        MiniDumpExplorer::MiniDumpPageParameters parameters{*this, tag, stream_index};
        item.Tag(parameters);
        const Controls::SymbolIcon iconSource;
        iconSource.Symbol(symbol);
        item.Icon(iconSource);
        return item;
    }
}
