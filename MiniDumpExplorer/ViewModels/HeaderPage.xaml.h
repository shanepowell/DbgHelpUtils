#pragma once

#include "HeaderPage.g.h"

#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPage;
    struct RecentFileItem;
    struct DumpFileTimeStamp;

    struct HeaderPage : HeaderPageT<HeaderPage>, GlobalOptionsNotifyPropertyChangedBase<HeaderPage>, MiniDumpPageBase<HeaderPage>
    {
        HeaderPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

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
        [[nodiscard]] MiniDumpExplorer::DumpFileTimeStamp TimeDateStamp() const { return timeDateStamp_; }
        [[nodiscard]] uint64_t Flags() const;
        [[nodiscard]] Windows::Foundation::Collections::IObservableVector<hstring> FlagsList() const { return flagsList_; }
        [[nodiscard]] MiniDumpExplorer::FileCrc32 DumpFileCrc32() const { return fileCrc32_; }

    private:
        void SetupFlyoutMenus();
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;
        void SetupMinidumpHeader(std::shared_ptr<dlg_help_utils::mini_dump> const& miniDump, hstring const& path, MiniDumpExplorer::FileCrc32 fileCrc32);
        fire_and_forget LoadFileItemIcon() const;

    private:
        MiniDumpExplorer::RecentFileItem fileItem_{nullptr};
        std::shared_ptr<dlg_help_utils::mini_dump> mini_dump_{};
        MiniDumpExplorer::DumpFileTimeStamp timeDateStamp_{};
        Windows::Foundation::Collections::IObservableVector<hstring> flagsList_{single_threaded_observable_vector<hstring>()};
        MiniDumpExplorer::FileCrc32 fileCrc32_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct HeaderPage : HeaderPageT<HeaderPage, implementation::HeaderPage>
    {
    };
}
