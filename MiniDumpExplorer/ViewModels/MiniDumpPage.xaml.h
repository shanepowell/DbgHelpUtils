#pragma once

#include "MiniDumpPage.g.h"
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

        void NavigationView_SelectionChanged(Microsoft::UI::Xaml::Controls::NavigationView const& sender, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);

    private:
        fire_and_forget LoadMiniDump(Windows::Storage::StorageFile const& file);

    private:
        Windows::Storage::StorageFile file_{ nullptr };
        std::unique_ptr<dlg_help_utils::mini_dump> miniDump_;
        std::wstring openError_;
        static std::unordered_map<std::wstring, Windows::UI::Xaml::Interop::TypeName> pageMap_;
        bool valid_{ false };
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpPage : MiniDumpPageT<MiniDumpPage, implementation::MiniDumpPage>
    {
    };
}
