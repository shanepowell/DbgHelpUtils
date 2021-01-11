#include "pch.h"
#include "MiniDumpMiscInfo5.h"

#include "../DbgHelpUtils/misc_info_stream.h"
#include "XStateConfigFeatureMscInfo.h"

namespace MiniDumpExplorerApp
{
    MiniDumpMiscInfo5::MiniDumpMiscInfo5(dlg_help_utils::misc_info_stream const& misc_info)
        : misc_info_{misc_info}
        , xstate_{*winrt::make_self<XStateConfigFeatureMscInfo>(misc_info_.misc_info_5().XStateData)}
    {
    }

    winrt::MiniDumpExplorer::IXStateConfigFeatureMscInfo MiniDumpMiscInfo5::XStateData() const
    {
        return xstate_;
    }

    bool MiniDumpMiscInfo5::HasProcessCookie() const
    {
        return (misc_info_.misc_info().Flags1 & MINIDUMP_MISC5_PROCESS_COOKIE) == MINIDUMP_MISC5_PROCESS_COOKIE;
    }

    uint32_t MiniDumpMiscInfo5::ProcessCookie() const
    {
        return misc_info_.misc_info_5().ProcessCookie;
    }
}
