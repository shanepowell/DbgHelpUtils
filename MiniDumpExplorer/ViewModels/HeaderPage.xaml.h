#pragma once

#include "HeaderPage.g.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPage;
    struct RecentFileItem;

    struct HeaderPage : HeaderPageT<HeaderPage>
    {
        HeaderPage();

        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);

        [[nodiscard]] MiniDumpExplorer::RecentFileItem FileItem() const { return fileItem_; }
        [[nodiscard]] hstring DumpType() const;
        [[nodiscard]] hstring Signature() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] bool IsUserDump() const;
        [[nodiscard]] bool IsKernelDump() const;
        [[nodiscard]] bool IsUnknownDump() const;
        [[nodiscard]] uint16_t Version() const;
        [[nodiscard]] uint16_t InternalVersion() const;
        [[nodiscard]] uint32_t NumberOfStreams() const;
        [[nodiscard]] uint32_t StreamDirectoryRva() const;
        [[nodiscard]] uint32_t CheckSum() const;
        [[nodiscard]] uint32_t DumpFileCrc32() const { return data_crc32_; }
        [[nodiscard]] uint32_t TimeDateStamp() const;
        [[nodiscard]] uint64_t Flags() const;
        [[nodiscard]] Windows::Foundation::Collections::IObservableVector<hstring> FlagsList() const { return flagsList_; }

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

    private:
        void SetupFlyoutMenus();
        void SetupGlobalOptionHooks();
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPage const& miniDumpPage);
        void SetupMinidumpHeader(dlg_help_utils::mini_dump const& miniDump, hstring const& path);
        void RaisePropertyChanged(hstring const& propertyName);
        fire_and_forget LoadFileItemIcon() const;
        void OnNumberDisplayFormatChanged();

    private:
        MiniDumpExplorer::RecentFileItem fileItem_{nullptr};
        dlg_help_utils::mini_dump const* mini_dump_{nullptr};
        uint32_t data_crc32_{0};
        Windows::Foundation::Collections::IObservableVector<hstring> flagsList_{single_threaded_observable_vector<hstring>()};
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_;
        event_token miniDumpLoadedEvent_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct HeaderPage : HeaderPageT<HeaderPage, implementation::HeaderPage>
    {
    };
}
