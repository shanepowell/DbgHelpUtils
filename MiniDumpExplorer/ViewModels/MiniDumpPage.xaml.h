#pragma once

#include "MiniDumpPage.g.h"

#include "DbgHelpUtils/mini_dump.h"

#include <memory>


namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPage : MiniDumpPageT<MiniDumpPage>
    {
        MiniDumpPage(Windows::Storage::StorageFile const& file);
        ~MiniDumpPage() override;

        MiniDumpPage(MiniDumpPage const&) = delete;
        MiniDumpPage(MiniDumpPage &&) = delete;

        MiniDumpPage& operator=(MiniDumpPage const&) = delete;
        MiniDumpPage& operator=(MiniDumpPage &&) = delete;

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        void NavigationView_SelectionChanged(Microsoft::UI::Xaml::Controls::NavigationView const& sender, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);

        [[nodiscard]] dlg_help_utils::mini_dump const& MiniDump() const { return *miniDump_; }
        [[nodiscard]] bool MiniDumpOpened() const { return miniDump_ != nullptr; }
        [[nodiscard]] Windows::Storage::StorageFile File() const { return file_; }

        Windows::Foundation::IAsyncOperation<bool> LoadMiniDump();

        void SelectNavigationItemTag(std::wstring const& tag);

        event_token MiniDumpLoaded(Windows::Foundation::EventHandler<MiniDumpExplorer::MiniDumpPage> const& value);
        void MiniDumpLoaded(event_token const& value);

    private:
        bool FindAndSelectNavigationTag(std::wstring const& tag, Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> const& menuItems);
        Microsoft::UI::Xaml::Controls::NavigationViewItem CreateNavigationViewItemForStreamType(MINIDUMP_STREAM_TYPE stream_type, uint32_t stream_index, MINIDUMP_LOCATION_DESCRIPTOR const& location) const;
        Microsoft::UI::Xaml::Controls::NavigationViewItem CreateNavigationViewItemForStream(std::wstring const& title, std::wstring const& tag, uint32_t stream_index, Microsoft::UI::Xaml::Controls::Symbol symbol) const;

    private:
        Windows::Storage::StorageFile file_{ nullptr };
        std::unique_ptr<dlg_help_utils::mini_dump> miniDump_;
        std::wstring openError_;
        static std::unordered_map<std::wstring, Windows::UI::Xaml::Interop::TypeName> pageMap_;
        bool valid_{ false };
        event<Windows::Foundation::EventHandler<MiniDumpExplorer::MiniDumpPage>> miniDumpLoadedHandler_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpPage : MiniDumpPageT<MiniDumpPage, implementation::MiniDumpPage>
    {
    };
}
