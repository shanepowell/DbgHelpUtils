#include "pch.h"
#include "StreamsPage.xaml.h"

#include "MiniDumpPage.xaml.h"
#include "Models/DirectoryStreamEntriesDataSource.h"
#include "Models/DirectoryStreamEntry.h"

#if __has_include("StreamsPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "StreamsPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPageParameters;

    StreamsPage::StreamsPage() = default;

    void StreamsPage::InitializeComponent()
    {
        StreamsPageT::InitializeComponent();

        onRowDoubleTapped_ = StreamsDataGrid().RowDoubleTapped({ this, &StreamsPage::OnRowDoubleTapped });
    }

    void StreamsPage::SelectStream([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
        SelectCurrentlySelectedStream();
    }

    void StreamsPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
        SelectCurrentlySelectedStream();
    }

    void StreamsPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        miniDumpPage_ = parameters.MiniDump();
        auto const miniDumpPage = miniDumpPage_.as<MiniDumpPage>();
        directoryStreamEntriesDataSource_.as<DirectoryStreamEntriesDataSource>()->LoadMiniDumpStreams(miniDumpPage->MiniDump());
    }

    void StreamsPage::SelectCurrentlySelectedStream() const
    {
        try
        {
            if (auto const selectedStream = directoryStreamEntriesDataSource_.CollectionView().CurrentItem();
                selectedStream)
            {
                if (auto const selectedStreamEntry = selectedStream.as<DirectoryStreamEntry>();
                    selectedStreamEntry)
                {
                    auto const miniDumpPage = miniDumpPage_.as<MiniDumpPage>();
                    miniDumpPage->SelectNavigationItemTag(selectedStreamEntry->StreamTypeTag());
                }
            }
        }
        catch(...)
        {
            logger::Log().HandleUnknownException();
        }
    }
}
