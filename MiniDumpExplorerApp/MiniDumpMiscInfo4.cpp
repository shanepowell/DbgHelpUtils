#include "pch.h"
#include "MiniDumpMiscInfo4.h"

#include "../DbgHelpUtils/misc_info_stream.h"

namespace MiniDumpExplorerApp
{
    MiniDumpMiscInfo4::MiniDumpMiscInfo4(dlg_help_utils::misc_info_stream const& misc_info)
        : misc_info_{misc_info}
    {
        if(HasBuildString())
        {
            build_string_ = misc_info_.misc_info_4().BuildString;
            debug_build_string_ = misc_info_.misc_info_4().DbgBldStr;
        }
    }

    bool MiniDumpMiscInfo4::HasBuildString() const
    {
        return (misc_info_.misc_info().Flags1 & MINIDUMP_MISC4_BUILDSTRING) == MINIDUMP_MISC4_BUILDSTRING;
    }

    winrt::hstring MiniDumpMiscInfo4::BuildString() const
    {
        return build_string_;
    }

    winrt::hstring MiniDumpMiscInfo4::DbgBldStr() const
    {
        return debug_build_string_;
    }
}
