#include "pch.h"
#include "MiniDumpMiscInfo2.h"

#include "../DbgHelpUtils/misc_info_stream.h"

namespace MiniDumpExplorerApp
{
    MiniDumpMiscInfo2::MiniDumpMiscInfo2(dlg_help_utils::misc_info_stream const& misc_info)
        : misc_info_{misc_info}
    {
    }

    bool MiniDumpMiscInfo2::HasProcessPowerInfo() const
    {
        return (misc_info_.misc_info().Flags1 & MINIDUMP_MISC1_PROCESSOR_POWER_INFO) == MINIDUMP_MISC1_PROCESSOR_POWER_INFO;
    }

    uint32_t MiniDumpMiscInfo2::ProcessorMaxMhz() const
    {
        return misc_info_.misc_info_2().ProcessorMaxMhz;
    }

    uint32_t MiniDumpMiscInfo2::ProcessorCurrentMhz() const
    {
        return misc_info_.misc_info_2().ProcessorCurrentMhz;
    }

    uint32_t MiniDumpMiscInfo2::ProcessorMhzLimit() const
    {
        return misc_info_.misc_info_2().ProcessorMhzLimit;
    }

    uint32_t MiniDumpMiscInfo2::ProcessorMaxIdleState() const
    {
        return misc_info_.misc_info_2().ProcessorMaxIdleState;
    }

    uint32_t MiniDumpMiscInfo2::ProcessorCurrentIdleState() const
    {
        return misc_info_.misc_info_2().ProcessorCurrentIdleState;
    }
}
