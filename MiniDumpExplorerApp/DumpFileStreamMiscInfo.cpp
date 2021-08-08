#include "pch.h"
#include "DumpFileStreamMiscInfo.h"

#include "GetResult.h"
#include "MiniDumpMiscInfo.h"
#include "MiniDumpMiscInfo2.h"
#include "MiniDumpMiscInfo3.h"
#include "MiniDumpMiscInfo4.h"
#include "MiniDumpMiscInfo5.h"

namespace MiniDumpExplorerApp
{
    DumpFileStreamMiscInfo::DumpFileStreamMiscInfo(size_t const index, dlg_help_utils::mini_dump const& mini_dump)
        : misc_info_{mini_dump, index}
    {
        if(!IsValid())
        {
            return;
        }

        info_ = *winrt::make_self<MiniDumpMiscInfo>(misc_info_);

        if(MiscInfoVersion() >= 2)
        {
            info2_ = *winrt::make_self<MiniDumpMiscInfo2>(misc_info_);
        }
        if(MiscInfoVersion() >= 3)
        {
            info3_ = *winrt::make_self<MiniDumpMiscInfo3>(misc_info_);
        }
        if(MiscInfoVersion() >= 4)
        {
            info4_ = *winrt::make_self<MiniDumpMiscInfo4>(misc_info_);
        }
        if(MiscInfoVersion() >= 5)
        {
            info5_ = *winrt::make_self<MiniDumpMiscInfo5>(misc_info_);
        }
    }

    uint64_t DumpFileStreamMiscInfo::Index() const
    {
        return misc_info_.index();
    }

    bool DumpFileStreamMiscInfo::IsValid() const
    {
        return misc_info_.is_valid();
    }

    uint64_t DumpFileStreamMiscInfo::MiscInfoVersion() const
    {
        return misc_info_.misc_info_version();
    }

    winrt::MiniDumpExplorer::IMiniDumpMiscInfo DumpFileStreamMiscInfo::MiscInfo() const
    {
        return GetResult(info_);
    }

    winrt::MiniDumpExplorer::IMiniDumpMiscInfo2 DumpFileStreamMiscInfo::MiscInfo2() const
    {
        return GetResult(info2_);
    }

    winrt::MiniDumpExplorer::IMiniDumpMiscInfo3 DumpFileStreamMiscInfo::MiscInfo3() const
    {
        return GetResult(info3_);
    }

    winrt::MiniDumpExplorer::IMiniDumpMiscInfo4 DumpFileStreamMiscInfo::MiscInfo4() const
    {
        return GetResult(info4_);
    }

    winrt::MiniDumpExplorer::IMiniDumpMiscInfo5 DumpFileStreamMiscInfo::MiscInfo5() const
    {
        return GetResult(info5_);
    }
}
