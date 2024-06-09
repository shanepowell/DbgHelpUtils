#pragma once

#include "Models/MiniDumpPageParameters.h"
#include "DbgHelpUtils/mini_dump.h"

class SubPageNavigationEntries
{
public:
    static void CreateNavigationViewItemEntriesForStream(MINIDUMP_STREAM_TYPE streamType, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem const& navigationViewItem, winrt::MiniDumpExplorer::MiniDumpPageParameters const& parameters, dlg_help_utils::mini_dump const& miniDump);
};
