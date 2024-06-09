#include "pch.h"
#include "SubPageNavigationEntries.h"

#include "ThreadListStreamPage.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace dlg_help_utils;

namespace
{
    std::unordered_map<MINIDUMP_STREAM_TYPE, std::function<void (Controls::NavigationViewItem const&, MiniDumpExplorer::MiniDumpPageParameters const&, mini_dump const&)>> StreamTypeToSubNavigationItems =
        {
            {ThreadListStream, MiniDumpExplorer::implementation::ThreadListStreamPage::CreateNavigationViewItemEntriesForStream},
        };
}

void SubPageNavigationEntries::CreateNavigationViewItemEntriesForStream(MINIDUMP_STREAM_TYPE const streamType, Controls::NavigationViewItem const& navigationViewItem, MiniDumpExplorer::MiniDumpPageParameters const& parameters, mini_dump const& miniDump)
{
    if (auto const it = StreamTypeToSubNavigationItems.find(streamType); it != StreamTypeToSubNavigationItems.end())
    {
        it->second(navigationViewItem, parameters, miniDump);
    }
}
