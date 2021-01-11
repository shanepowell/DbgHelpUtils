#include "pch.h"
#include "MiniDumpMiscInfo.h"

#include "../DbgHelpUtils/misc_info_stream.h"

namespace MiniDumpExplorerApp
{
    MiniDumpMiscInfo::MiniDumpMiscInfo(dlg_help_utils::misc_info_stream const& misc_info)
        : misc_info_{misc_info}
    {
    }

    uint32_t MiniDumpMiscInfo::Flags1() const
    {
        return misc_info_.misc_info().Flags1;
    }

    bool MiniDumpMiscInfo::HasProcessId() const
    {
        return (misc_info_.misc_info().Flags1 & MINIDUMP_MISC1_PROCESS_ID) == MINIDUMP_MISC1_PROCESS_ID;
    }

    uint32_t MiniDumpMiscInfo::ProcessId() const
    {
        return misc_info_.misc_info().ProcessId;
    }

    bool MiniDumpMiscInfo::HasProcessTimes() const
    {
        return (misc_info_.misc_info().Flags1 & MINIDUMP_MISC1_PROCESS_TIMES) == MINIDUMP_MISC1_PROCESS_TIMES;
    }

    winrt::clock::time_point MiniDumpMiscInfo::ProcessCreateTime() const
    {
        return winrt::clock::from_time_t(misc_info_.misc_info().ProcessCreateTime);
    }

    uint32_t MiniDumpMiscInfo::ProcessUserTime() const
    {
        return misc_info_.misc_info().ProcessUserTime;
    }

    uint32_t MiniDumpMiscInfo::ProcessKernelTime() const
    {
        return misc_info_.misc_info().ProcessKernelTime;
    }
}
