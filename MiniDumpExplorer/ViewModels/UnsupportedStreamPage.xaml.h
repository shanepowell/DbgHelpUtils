#pragma once

#include "UnsupportedStreamPage.g.h"

#include "Models/MiniDumpPageBase.h"
#include "Models/NotifyPropertyChangedBase.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct UnsupportedStreamPage : UnsupportedStreamPageT<UnsupportedStreamPage>, NotifyPropertyChangedBase<UnsupportedStreamPage>, MiniDumpPageBase<UnsupportedStreamPage>
    {
        UnsupportedStreamPage() = default;

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        hstring HexDump() const { return streamDataHexDump_; }

        uint32_t Index() const { return index_; }

    protected:
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        uint32_t index_{};
        hstring streamDataHexDump_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct UnsupportedStreamPage : UnsupportedStreamPageT<UnsupportedStreamPage, implementation::UnsupportedStreamPage>
    {
    };
}
