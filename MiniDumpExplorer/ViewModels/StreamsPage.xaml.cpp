#include "pch.h"
#include "StreamsPage.xaml.h"
#include "MiniDumpPage.xaml.h"

#include "Models/DirectoryStreamEntriesDataSource.h"

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

    void StreamsPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto const miniDumpPageParameters = e.Parameter().as<MiniDumpExplorer::MiniDumpPageParameters>();
        if(auto const miniDumpPage = miniDumpPageParameters.MiniDump().as<MiniDumpPage>();
            miniDumpPage->MiniDumpOpened())
        {
            MiniDumpLoaded(miniDumpPage->MiniDump());
        }
        else
        {
            miniDumpLoadedEvent_ = miniDumpPage->MiniDumpLoaded([weakThis{ get_weak() }]([[maybe_unused]] auto const& sender, auto const& args)
            {
                if (auto strongThis{ weakThis.get() }; strongThis)
                {
                    auto const internalMiniDumpPage = args.template as<MiniDumpPage>();
                    strongThis->MiniDumpLoaded(internalMiniDumpPage->MiniDump());
                }
            });
        }
    }


    void StreamsPage::SelectStream([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    void StreamsPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void StreamsPage::MiniDumpLoaded(dlg_help_utils::mini_dump const& miniDump)
    {
        mini_dump_ = &miniDump;
        directoryStreamEntriesDataSource_.as<DirectoryStreamEntriesDataSource>()->LoadMiniDumpStreams(*mini_dump_);
    }
}
