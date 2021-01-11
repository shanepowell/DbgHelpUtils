#include "pch.h"
#include "MiniDumpMiscInfo3.h"

#include "GetResult.h"
#include "TimeZoneInformation.h"
#include "../DbgHelpUtils/misc_info_stream.h"
#include "../DbgHelpUtils/system_info_utils.h"

namespace MiniDumpExplorerApp
{
    MiniDumpMiscInfo3::MiniDumpMiscInfo3(dlg_help_utils::misc_info_stream const& misc_info)
        : misc_info_{misc_info}
        , process_integrity_level_{dlg_help_utils::system_info_utils::process_integrity_level_to_string(misc_info_.misc_info_3().ProcessIntegrityLevel)}
    {
        if(HasProcessExecuteFlags())
        {
            for (auto const& value : dlg_help_utils::system_info_utils::process_execute_flags_to_strings(misc_info_.misc_info_3().ProcessExecuteFlags))
            {
                process_execute_flags_.Append(winrt::hstring{value});
            }
        }

        if(HasTimeZone())
        {
            time_zone_ = *winrt::make_self<TimeZoneInformation>(misc_info_.misc_info_3().TimeZone);
        }
    }

    bool MiniDumpMiscInfo3::HasProcessIntegrityLevel() const
    {
        return (misc_info_.misc_info().Flags1 & MINIDUMP_MISC3_PROCESS_INTEGRITY) == MINIDUMP_MISC3_PROCESS_INTEGRITY;
    }

    uint32_t MiniDumpMiscInfo3::ProcessIntegrityLevel() const
    {
        return misc_info_.misc_info_3().ProcessIntegrityLevel;
    }

    winrt::hstring MiniDumpMiscInfo3::ProcessIntegrityLevelString() const
    {
        return process_integrity_level_;
    }

    bool MiniDumpMiscInfo3::HasProcessExecuteFlags() const
    {
        return (misc_info_.misc_info().Flags1 & MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS) == MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS;
    }

    uint32_t MiniDumpMiscInfo3::ProcessExecuteFlags() const
    {
        return misc_info_.misc_info_3().ProcessExecuteFlags;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> MiniDumpMiscInfo3::ProcessExecuteFlagStrings() const
    {
        return process_execute_flags_;
    }

    bool MiniDumpMiscInfo3::HasProtectedProcess() const
    {
        return (misc_info_.misc_info().Flags1 & MINIDUMP_MISC3_PROTECTED_PROCESS) == MINIDUMP_MISC3_PROTECTED_PROCESS;
    }

    uint32_t MiniDumpMiscInfo3::ProtectedProcess() const
    {
        return misc_info_.misc_info_3().ProtectedProcess;
    }

    bool MiniDumpMiscInfo3::HasTimeZone() const
    {
        return (misc_info_.misc_info().Flags1 & MINIDUMP_MISC3_TIMEZONE) == MINIDUMP_MISC3_TIMEZONE;
    }

    uint32_t MiniDumpMiscInfo3::TimeZoneId() const
    {
        return misc_info_.misc_info_3().TimeZoneId;
    }

    winrt::MiniDumpExplorer::ITimeZoneInformation MiniDumpMiscInfo3::TimeZone() const
    {
        return GetResult(time_zone_);
    }
}
